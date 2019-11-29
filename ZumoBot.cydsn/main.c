#include "main.h"


bool calibrated = false;
static uint8_t speed = 100;
tile t;
mqtt_message msg = {"", ""};
int action = 0;
const char mqtt_version[] = "v0.1.1";


int zmain(void) {
    BatteryLed_Write(1);
    
    CyGlobalIntEnable;
    Button_Interrupt_StartEx(button_isr);
    LED_Interrupt_StartEx(led_isr);
    
    reflectance_start();
    UART_1_Start(); 
    vl53l0x_init();
    LED_Timer_Start();
    
    mqtt_sub("Ctrl/Net/#");
    mqtt_print("Ctrl/Zumo/Status", "Ready");
    change_state(BOOT_IDLE_STATE);
    
    while (1) {
        switch (current_state) {
            case BOOT_IDLE_STATE:
                vTaskDelay(1000);
            break;
                
            case CLB_IDLE_STATE:
                if (!calibrated) { 
                    calibrate_ref();
                    calibrated = true;
                }
                vTaskDelay(1000);
            break;
            
            case WAIT_STATE:
                if (mqtt_receive(&msg)) {
                    if (!strcmp(msg.topic, "Ctrl/Net/Status") && !strcmp(msg.message, "Ready")) {
                        mqtt_print("Ctrl/Zumo/Version", "%s", mqtt_version);
                        mqtt_print("Ack/Zumo" , "Ready");
                    }
                    
                    if (!strcmp(msg.topic, "Ctrl/Net/Version")) {
                        mqtt_print("Ack/Zumo" , "Version");
                        if (!strcmp(msg.message, mqtt_version)) {
                            mqtt_print("Info/Zumo/Connect" , "Server found");
                            change_state(PRESCAN_STATE);
                        } else {
                            mqtt_print("Info/Zumo/WARNING" , "Incompatible");
                            change_state(ERR_STATE);
                        }
                        break;
                    }
                }
                vTaskDelay(100);
            break;   
                
            case PRESCAN_STATE:
                vTaskDelay(2000);
                mqtt_print("Info/Zumo/Scan", "Pre-scan started");
                
                move_to_next(speed);
                move_to_next(speed);
                pre_scan(speed);
                
                change_state(NAV_STATE);
                mqtt_print("Ctrl/Zumo/Move", "-2");
            break;
            
            case NAV_STATE:
                if (mqtt_receive(&msg)) { 
                    if (!strcmp(msg.topic, "Ctrl/Net/Status")) {
                        if (!strcmp(msg.message, "Finish")) {
                            mqtt_print("Ack/Zumo", "Finish");
                            change_state(CMP_NAV_STATE);
                        } else if (!strcmp(msg.message, "Stuck")) {
                            mqtt_print("Ack/Zumo", "Stuck");
                            change_state(FIN_IDLE_STATE);
                        }
                      
                    } else if (!strcmp(msg.topic, "Ctrl/Net/Action")) {
                        sscanf(msg.message, "%i", &action);
                        mqtt_print("Ack/Zumo", "Action");
                        
                        rotate_to(action, speed);
                        int dist = scan(&t);
                        send_obstacle(t);
                        
                        if (dist < 0 || dist > 1) {
                            mqtt_print("Ctrl/Zumo/Move", "%i", action);
                            move_to_next(speed);
                        } else {
                            mqtt_print("Ctrl/Zumo/Move", "-1");
                        }
                    }
                }
            break;
            
            case CMP_NAV_STATE:
                complete_track(speed);
                change_state(FIN_IDLE_STATE);
            break;
            
            case FIN_IDLE_STATE:
                vTaskDelay(1000);
            break;
            
            case LOCK_STATE:
                vTaskDelay(1000);
            break;
            
            case ERR_STATE:
                vTaskDelay(1000);
            break;
        
            default:
                mqtt_print("Info/Zumo/WARNING", "Undef state!");
                change_state(ERR_STATE);
            break;
        }
    }
}


CY_ISR(button_isr) {
    if (current_state == BOOT_IDLE_STATE) {
        change_state(CLB_IDLE_STATE);
    } else if (current_state == CLB_IDLE_STATE) {
        change_state(WAIT_STATE);
    } else {
        if (motor_enabled() && states[current_state].movement_enabled) {
            change_state(LOCK_STATE);
        } else if (current_state == 7) {
            change_state(PREV_STATE);
        }
    }
    SW1_ClearInterrupt();
}


CY_ISR(led_isr) {
    ++led_state;
    if (!states[find_state(current_state)].led_timings[led_state]) {
        led_state = 0;
    }
    if (led_state % 2) {
        BatteryLed_Write(1);
    } else {
        BatteryLed_Write(0);
    }
    
    LED_Timer_WritePeriod(states[find_state(current_state)].led_timings[led_state]);
    LED_Timer_ClearFIFO();
}


/* [] END OF FILE */

#include "main.h"


int zmain(void) {
    BatteryLed_Write(1);
    
    CyGlobalIntEnable;
    Button_Interrupt_StartEx(button_isr);
    LED_Interrupt_StartEx(led_isr);
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    LED_Timer_Start();
    while(voltage_test());  // Wait for power to continue
    
    reflectance_start();
    UART_1_Start(); 
    vl53l0x_init();
    
    mqtt_sub("Net/#");
    mqtt_print("Zumo/Status", "Boot");
    change_state(BOOT_IDLE_STATE);
    
    while (1) {
        voltage_check();
        
        switch (current_state) {
            case BOOT_IDLE_STATE: 
                vTaskDelay(1000);
            break;
                
            case CLB_IDLE_STATE:
                if (!calibrated) { 
                    calibrate();
                    calibrated = true;
                }
            break;
            
            case WAIT_STATE:
                if (mqtt_receive(&msg) && strstr(msg.topic, "Net/Status") && strstr(msg.message, "Ready")) {
                    change_state(PRESCAN_STATE);
                    mqtt_print("Ack/Zumo" , "Ready");
                    mqtt_print("Zumo/Status", "Ready");
                }
                vTaskDelay(1000);
            break;   
                
            case PRESCAN_STATE:
                vTaskDelay(5000);
                mqtt_print("Info/Zumo", "Pre-scan started");
                
                move_to_next(speed);
                move_to_next(speed);
                pre_scan(speed);
                
                mqtt_print("Zumo/Status", "Ready");
                change_state(NAV_STATE);
                vTaskDelay(1000);
            break;
            
            case NAV_STATE:
                if (mqtt_receive(&msg)) { 
                    mqtt_print("Rec/Zumo", "Rec on \"%s\"", msg.topic);
                    if (strstr(msg.topic, "Net/Status")) {
                        if (strstr(msg.message, "Finish")) {
                            mqtt_print("Ack/Zumo", "Finish");
                            change_state(CMP_NAV_STATE);
                        } else if (strstr(msg.message, "Stuck")) {
                            mqtt_print("Ack/Zumo", "Stuck");
                            change_state(FIN_IDLE_STATE);
                        }
                      
                    } else if (strstr(msg.topic, "Net/Action")) {
                        sscanf(msg.message, "%i", &action);
                        mqtt_print("Ack/Zumo", "Action");
                        
                        rotate_to(action, speed);
                        int dist = scan(&t);
                        send_obstacle(t);
                        
                        if (dist < 0 || dist > 1) {
                            move_to_next(speed);
                            mqtt_print("Zumo/Move", "%i", action);
                        } else {
                            mqtt_print("Zumo/Move", "-1");
                        }
                    }
                }
            break;
            
            case CMP_NAV_STATE:
                complete_track(speed);
                change_state(FIN_IDLE_STATE);
                vTaskDelay(1000);
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


void print_element(const void *element) {
    
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
    if (!states[current_state].led_timings[led_state]) {
        led_state = 0;
    }
    if (led_state % 2) {
        BatteryLed_Write(1);
    } else {
        BatteryLed_Write(0);
    }
    
    LED_Timer_WritePeriod(states[current_state].led_timings[led_state]);
    LED_Timer_ClearFIFO();
}


int voltage_check() {
    if (voltage_test() && !low_voltage_detected) {
        mqtt_print("Info/Zumo/WARNING", "Low voltage: %3.2fV!", battery_voltage());
        low_voltage_detected = true;
        change_state(ERR_STATE);
    } else if (!voltage_test() && low_voltage_detected) {
        mqtt_print("Info/Zumo/Status", "Voltage normal");
        low_voltage_detected = false;
        change_state(PREV_STATE);
    }
    return voltage_test();
}


/* [] END OF FILE */

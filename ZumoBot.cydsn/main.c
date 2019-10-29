#include "main.h"


int zmain(void) {
    BatteryLed_Write(1);
    
    CyGlobalIntEnable;
    Button_Interrupt_StartEx(button_isr);
    LED_Interrupt_StartEx(led_isr);
    
    reflectance_start();
    UART_1_Start(); 
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    LED_Timer_Start();
    vl53l0x_init();
    
    mqtt_sub("Net/#");
    mqtt_print("Zumo/Status", "Boot");
    change_state(0);
    
    while (1) {
        voltage_check();
        
        switch (current_state) {
            case 0: 
                vTaskDelay(1000);
            break;
                
            case 1:
                if (!calibrated) { 
                    calibrate();
                    calibrated = true;
                }
            break;
            
            case 2:
                if (mqtt_receive(&msg) && strstr(msg.topic, "Net/Status") && strstr(msg.message, "Ready")) {
                    change_state(3);
                    mqtt_print("Ack/Zumo" , "Ready");
                    mqtt_print("Zumo/Status", "Ready");
                }
                vTaskDelay(1000);
            break;   
                
            case 3:
                if (motor_enabled()) {
                    vTaskDelay(5000);
                    mqtt_print("Info/Zumo", "Pre-scan started");
                    
                    move_to_next(speed);
                    move_to_next(speed);
                    pre_scan(speed);
                }
                
                if (motor_enabled()) {
                    mqtt_print("Zumo/Status", "Ready");
                    change_state(4);
                }
                vTaskDelay(1000);
            break;
            
            case 4:
                if (mqtt_receive(&msg)) { 
                    mqtt_print("Rec/Zumo", "Rec on \"%s\"", msg.topic);
                    if (strstr(msg.topic, "Net/Status")) {
                        if (strstr(msg.message, "Finish")) {
                            mqtt_print("Ack/Zumo", "Finish");
                            change_state(5);
                        } else if (strstr(msg.message, "Stuck")) {
                            mqtt_print("Ack/Zumo", "Stuck");
                            change_state(6);
                        }
                      
                    } else if (strstr(msg.topic, "Net/Action")) {
                        sscanf(msg.message, "%i", &action);
                        mqtt_print("Ack/Zumo", "Action");
                        rotate_to(action, speed);
                        int dist = scan(&t);
                        send_obstacle(t);
                        if (dist > 1) {
                            move_to_next(speed);
                            mqtt_print("Zumo/Move", "%i", action);
                        } else {
                            mqtt_print("Zumo/Move", "-1");
                        }
                        scan(&t);
                        send_obstacle(t);
                        
                    }
                }
            break;
            
            case 5:
                complete_track(speed);
                change_state(6);
                vTaskDelay(1000);
            break;
            
            case 6:
                vTaskDelay(1000);
            break;
            
            case 7:
                vTaskDelay(1000);
            break;
            
            case 8:
                vTaskDelay(1000);
            break;
        
            default:
                mqtt_print("Info/Zumo/WARNING", "Undef state!");
                change_state(8);
            break;
        }
    }
}


void print_element(const void *element) {
    
}


void change_state(int state) {
    if (state >= 0) {
        if (state != current_state) {
            prev_state = current_state;
            current_state = state;
        }
    } else {
        int t = prev_state;
        prev_state = current_state;
        current_state = t;
    }
    set_motor_state(states[current_state].movement_enabled);
    
    mqtt_print("Info/Zumo/State", "%s state (%i)", states[current_state].name, current_state);
    led_state = 0;
}


CY_ISR(button_isr) {
    if (current_state == 0) {
        change_state(1);
    } else if (current_state == 1) {
        change_state(2);
    } else {
        if (motor_enabled() && states[current_state].movement_enabled) {
            change_state(7);
        } else if (current_state == 7) {
            change_state(-1);
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


void voltage_check() {
    if (!voltage_test() && !low_voltage_detected) {
        mqtt_print("Info/Zumo/WARNING", "Low voltage!");
        low_voltage_detected = true;
        change_state(8);
    } else if (voltage_test() && low_voltage_detected) {
        mqtt_print("Info/Zumo/Status", "Voltage normal");
        low_voltage_detected = false;
        change_state(-1);
    }
}


/* [] END OF FILE */

#include "main.h"


int zmain(void) {
    CyGlobalIntEnable;  // Enable global interrupts.
    Button_Interrupt_StartEx(button_isr);  // Link button interrupt to isr
    LED_Interrupt_StartEx(led_isr);
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    LED_Timer_Start();
    BatteryLed_Write(1);
    
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
                if (motor_enabled()) {
                    vTaskDelay(5000);
                    
                    mqtt_print("Info/Zumo", "Pre-scan started");
                    pre_scan(speed);
                }
                
                if (motor_enabled()) {
                    mqtt_print("Zumo/Status", "Ready");
                    change_state(3);
                }
                vTaskDelay(1000);
            break;
            
            case 3:
                if (mqtt_receive(&msg) && strstr(msg.topic, "Net/Status") && strstr(msg.message, "Ready")) {
                    change_state(4);
                    printf("Received net ready confirmation\n");
                    mqtt_print("Ack/Zumo" , "Ready");
                    mqtt_print("Zumo/Status", "Ready");
                }
                vTaskDelay(1000);
            break;
            
            case 4:
                if (mqtt_receive(&msg)) { 
                    mqtt_print("Rec/Zumo", "Rec on \"%s\"", msg.topic);
                    if (strstr(msg.topic, "Net/Status")) {
                        if (strstr(msg.message, "Finish")) {
                            printf("Received finish confirmation\n");
                            mqtt_print("Ack/Zumo", "Finish");
                            change_state(5);
                        } else if (strstr(msg.message, "Stuck")) {
                            printf("Received stuck confirmation\n");
                            mqtt_print("Ack/Zumo", "Stuck");
                            change_state(5);
                        }
                      
                    } else if (strstr(msg.topic, "Net/Action")) {
                        sscanf(msg.message, "%i", &action);
                        printf("Received an order to execute action %i\n", action);
                        mqtt_print("Ack/Zumo", "Action");
                        rotate_and_center(action, speed);
                        move_to_next_intersection(speed);
                        t = scan();
                        send_obstacle(t);
                        
                        mqtt_print("Zumo/Move", "%i", action);
                    }
                }
            break;
            
            case 5:
                vTaskDelay(1000);
            break;
            
            case 6:
                vTaskDelay(1000);
            break;
            
            case 7:
                vTaskDelay(1000);
            break;
        
            default:
                printf("Tried to enter undefined state!\n");
                mqtt_print("Info/Zumo/WARNING", "Undef state!");
                change_state(7);
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
    if (states[current_state].movement_enabled) {
        set_motor_state(1);
    } else {
        set_motor_state(0);
    }
    
    printf("Entered %s state (%i)\n", states[current_state].name, current_state);
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
            change_state(6);
        } else if (current_state  == 6) {
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
            printf("Low voltage. Disabling motors, entering error state\n");
            low_voltage_detected = true;
            change_state(7);
            vTaskDelay(5000);
        } else if (voltage_test() && low_voltage_detected) {
            mqtt_print("Info/Zumo/Status", "Voltage normal");
            printf("Voltage normal. Enabling motors, restoring state\n");
            low_voltage_detected = false;
            change_state(-1);
        }
}


/* [] END OF FILE */

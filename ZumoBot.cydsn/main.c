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
    mqtt_print("Zumo/Boot", "Zumo booted");
    printf("Booted. Entering booted idle state\n");
    
    while (1) {  
        t = xTaskGetTickCount();
        if (!voltage_test()) {
            if (!low_voltage_detected) {
                mqtt_print("Zumo/WARNING", "Low voltage!");
                printf("Low voltage. Disabling motors, entering error state\n");
                low_voltage_detected = true;
            }
            led_state[0] = 7;
            set_motor_state(0);
            vTaskDelay(1000);
        } else if (voltage_test() && low_voltage_detected) {
            mqtt_print("Zumo/DEBUG", "Voltage normal");
            printf("Voltage normal. Enabling motors\n");
            low_voltage_detected = false;
            set_motor_state(1);
        }
        
        if (calibration_mode) {
            led_state[0] = 1;
            printf("Calibrated. Entering calibrated idle state\n");;
            calibrate();
            calibration_mode = false;
            calibration_done = true;
            pre_scan = true;
        }
        
        if (motor_enabled() && pre_scan) {
            led_state[0] = 2;
            printf("Motors enabled. Entering pre-start scanning state\n");
            
            vTaskDelay(5000);
            // Pre-scan goes here
            mqtt_print("Zumo/Start", "Start");
            pre_scan = false;
        }
        
        if (motor_enabled()) {
            if (led_state[0] !=3 && led_state[0] !=4) {
                led_state[0] = 3;
                printf("Pre-scan done. Entering server wait state\n");
            }
            
            if (mqtt_receive(&msg)) {
                if (led_state[0] != 4) {
                    led_state[0] = 4;
                    printf("Message received from server. Entering server-guided state\n");
                }
                
                if (strstr(msg.topic, "Net/Status")) {
                    if (strstr(msg.message, "Ready")) {
                        printf("Received net ready confirmation. Resending start confirmation\n");
                        mqtt_print("Zumo/Start", "Start");
                    } else if (strstr(msg.message, "Finish")) {
                        printf("Received finish confirmation. "
                               "Entering finished idle state, disabling motors\n");
                        set_motor_state(0);
                        led_state[0] = 5;
                        pre_scan = true;
                    } else if (strstr(msg.message, "Stuck")) {
                        printf("Received stuck confirmation. "
                               "Entering finished idle state, disabling motors\n");
                        set_motor_state(0);
                        led_state[0] = 5;
                        pre_scan = true;
                    }
                  
                } else if (strstr(msg.topic, "Net/Action")) {
                    sscanf(msg.message, "%i", &action);
                    printf("Received an order to execute action %i\n", action);
//                    rotate(&robot_state, action, speed);
//                    move_to_next_intersection(&robot_state, speed);
                    vTaskDelay(1000);
                    mqtt_print("Zumo/Move", "%i", t);
                }
            }
        }
    }
}


void print_element(const void *element) {
    
}


CY_ISR(button_isr) {
    if (calibration_done) {
        if (motor_enabled()) {
            set_motor_state(0);
            led_state[0] = 6;
        } else {
            set_motor_state(1);
        }
    } else {
        calibration_mode = true;
    }
    SW1_ClearInterrupt();
}


CY_ISR(led_isr) {
    ++led_state[1];
    if (!led_timings[led_state[0]][led_state[1]]) {
        led_state[1] = 0;
    }
    if (led_state[1] % 2) {
        BatteryLed_Write(1);
    } else {
        BatteryLed_Write(0);
    }
    
    LED_Timer_WritePeriod(led_timings[led_state[0]][led_state[1]]);
    LED_Timer_ClearFIFO();
}


/* [] END OF FILE */

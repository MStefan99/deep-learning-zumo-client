#include "main.h"


int zmain(void) {
    CyGlobalIntEnable;  // Enable global interrupts.
    Button_isr_StartEx(Button_Interrupt);  // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    
    mqtt_sub("Net/#");
    printf("Sending start confirmation\n");
    mqtt_print("Zumo/Start", "Start");
    
    while (1) {  
        t = xTaskGetTickCount();
        if (!voltage_test() && !low_voltage_detected) {
            mqtt_print("Zumo/WARNING", "Low voltage!");
            low_voltage_detected = true;
            PWM_Stop();
            vTaskDelay(10000);
        } else if (voltage_test() && low_voltage_detected) {
            mqtt_print("Zumo/DEBUG", "Voltage normal");
            low_voltage_detected = false;
            PWM_Start();
        }
        
        if(calibration_mode) {
            calibrate();
            calibration_mode = false;
            calibration_done = true;
        }
        
        if (motor_enabled()) {
            if (mqtt_receive(&msg)) {
                if (strstr(msg.topic, "Net/Status")) {
                    if (strstr(msg.message, "Ready")) {
                        printf("Received net ready confirmation, resending start confirmation\n");
                        mqtt_print("Zumo/Start", "Start");
                    } else if (strstr(msg.message, "Finish")) {
                        printf("Received finish confirmation, disabling motors\n");
                        set_motor_state(0);
                    } else if (strstr(msg.message, "Stuck")) {
                        printf("Received stuck confirmation, disabling motors\n");
                        set_motor_state(0);
                    }
                  
                } else if (strstr(msg.topic, "Net/Action")) {
                    sscanf(msg.message, "%i", &action);
                    printf("Received an order to execute action %i\n", action);
                    rotate(&robot_state, action, speed);
                    move_to_next_intersection(&robot_state, speed);
                    mqtt_print("Zumo/Move", "%i", t);
                }
            }
        }
    }
}


void print_element(const void *element) {
    
}


CY_ISR(Button_Interrupt) {
    if (calibration_done) {
        if (motor_enabled()) {
            set_motor_state(0);
            BatteryLed_Write(0);
        } else {
            set_motor_state(1);
            BatteryLed_Write(1);
        }
    } else {
        calibration_mode = true;
    }
    SW1_ClearInterrupt();
}


/* [] END OF FILE */

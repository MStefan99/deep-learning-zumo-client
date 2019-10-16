#include "main.h"


int zmain(void) {
    CyGlobalIntEnable;  // Enable global interrupts.
    Button_isr_StartEx(Button_Interrupt);  // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    IR_Start();
    mqtt_print("Zumo/ready", "Zumo setup done");
    mqtt_sub("test/#");
    
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
        
        mqtt_print("test/testing", "this will be received");
        mqtt_print("testing/test", "this will not");
        
        mqtt_message msg;
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

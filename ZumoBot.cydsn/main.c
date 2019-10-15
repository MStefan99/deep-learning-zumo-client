#include <project.h>
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "Motor.h"
#include "Ultra.h"
#include "Nunchuk.h"
#include "Reflectance.h"
#include "Gyro.h"
#include "Accel_magnet.h"
#include "LSM303D.h"
#include "IR.h"
#include "Beep.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include "voltage.h"
#include "line_detection.h"
#include "log.h"
#include "movement.h"
#include "smqtt.h"


static uint8_t speed = 100;
volatile bool calibration_mode = false;
volatile bool calibration_done = false;


CY_ISR_PROTO(Button_Interrupt);
void print_element(const void *element);


int zmain(void) {   
    bool low_voltage_detected = false;
    
    CyGlobalIntEnable;  // Enable global interrupts.
    Button_isr_StartEx(Button_Interrupt);  // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    IR_Start();
    mqtt_print("Zumo/ready", "Zumo setup done");
    
    while (1) {  
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
        
        mqtt_sub("test/#");
        mqtt_print("test/testing", "this will be received");
        mqtt_print("testing/test", "this will not");
        
        mqtt_message msg;
        if (mqtt_receive(&msg)) {
            printf("Received message on topic \"%s\": \"%s\"\n", msg.topic, msg.message);
        }
        vTaskDelay(2000);
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

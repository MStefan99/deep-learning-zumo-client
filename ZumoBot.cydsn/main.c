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
#include "mqtt_sender.h"
#include <time.h>
#include <sys/time.h>
#include "serial1.h"
#include <unistd.h>
#include "voltage.h"
#include "line_detection.h"
#include "log.h"
#include "movement.h"
#include "mqtt_receiver.h"


static uint8_t speed = 100;
volatile bool calibration_mode = false;
volatile bool calibration_done = false;


CY_ISR_PROTO(Button_Interrupt);
void print_element(const void *element);


int zmain(void) {   
    bool low_voltage_detected = false;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    IR_Start();
    print_mqtt("Zumo/ready", "Zumo setup done");
    
    for (;;) {  
        if (!voltage_test() && !low_voltage_detected) {
            print_mqtt("Zumo/WARNING", "Low voltage!");
            low_voltage_detected = true;
            PWM_Stop();
            vTaskDelay(10000);
        } else if (voltage_test() && low_voltage_detected) {
            print_mqtt("Zumo/DEBUG", "Voltage normal");
            low_voltage_detected = false;
            PWM_Start();
        }
        
        if(calibration_mode) {
            calibrate();
            calibration_mode = false;
            calibration_done = true;
        } 
        
        if (motor_enabled()) {
            move_to_next_intersection(speed);
            rotate(left, speed);
            move_to_next_intersection(speed);
            rotate(forward, speed);
            move_to_next_intersection(speed);
            rotate(right, speed);
            move_to_next_intersection(speed);
            rotate(forward, speed);
            move_to_next_intersection(speed);
            rotate(backward, speed);
            move_to_next_intersection(speed);
            set_motor_state(0);
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

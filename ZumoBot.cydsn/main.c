
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
#include <voltage.h>
#include <line_detection.h>
#include <log.h>
#include <movement.h>

/**
 * @file    main.c
 * @brief   
 * @details  ** Enable global interrupt since Zumo library uses interrupts. **<br>&nbsp;&nbsp;&nbsp;CyGlobalIntEnable;<br>
*/
typedef enum {
    forward,
    right,
    back, 
    left
} robot_direction;


typedef struct {
    int x;
    int y;
    robot_direction direction;
} robot_position; 


static uint8_t speed = 50;
bool movement_allowed = false;
volatile bool calibration_mode = false;
volatile bool calibration_done = false;


CY_ISR_PROTO(Button_Interrupt);


int zmain(void)
{   
    reflectance_offset reflectance_offset = {0,0,0};
    sensors reflectance_values;
    bool reflectance_black = false;
    bool low_voltage_detected = false;
    uint8_t cross_count = 0;
    int line_shift_change;
    int line_shift;
    int shift_correction;
    float p_coefficient = 2.5;
    float d_coefficient = 4;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    Button_isr_StartEx(Button_Interrupt); // Link button interrupt to isr
    
    reflectance_start();
    UART_1_Start(); 
    Ultra_Start();
    ADC_Battery_Start();
    ADC_Battery_StartConvert();
    print_mqtt("Zumo/ready", "Zumo setup done");
    PWM_Start();
    IR_Start();
    
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
        
        if (!cross_detected()) {
            if(reflectance_black) {
                ++cross_count;
            }
            reflectance_black = false;
        } else {
            reflectance_black = true;
        }
        
        if(calibration_mode) {
            reflectance_read(&reflectance_values);
            reflectance_offset = reflectance_calibrate(&reflectance_values);
            calibration_mode = false;
            calibration_done = true;
        } 
        
        reflectance_read(&reflectance_values);
        reflectance_normalize(&reflectance_values, &reflectance_offset);
        
        line_shift = get_offset(&reflectance_values);
        line_shift_change = get_offset_change(&reflectance_values);        
        shift_correction = line_shift * p_coefficient + line_shift_change * d_coefficient;
        
        if (movement_allowed) {
        }
        
        if (0) {
            print_mqtt("Zumo/Status", "Calibrated: %i", calibration_done);
            print_mqtt("Zumo/Status", "Mov. allowed: %i", movement_allowed);
            print_mqtt("Zumo/Status", "Speed: %i", speed);
            
            print_mqtt("Zumo/Status", "Dist: %i", Ultra_GetDistance());
            print_mqtt("Zumo/Status", "R3: %i", reflectance_values.r3);
            print_mqtt("Zumo/Status", "R2: %i", reflectance_values.r2);
            print_mqtt("Zumo/Status", "R1: %i", reflectance_values.r1);
            print_mqtt("Zumo/Status", "L1: %i", reflectance_values.l1);
            print_mqtt("Zumo/Status", "L2: %i", reflectance_values.l2);
            print_mqtt("Zumo/Status", "L3: %i\n", reflectance_values.l3);
            vTaskDelay(2000);
        }
    }
}


CY_ISR(Button_Interrupt) {
    if (calibration_done) {
        if (movement_allowed) {
            speed = 200;
        } else {
            movement_allowed = true;
            speed = 75;
        }
    }
    
    calibration_mode = true;
    SW1_ClearInterrupt();
}


/* [] END OF FILE */

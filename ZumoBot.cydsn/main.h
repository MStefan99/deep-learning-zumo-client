/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#ifndef MAIN
    #define MAIN
    
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
    #include <stdbool.h>


    volatile bool calibration_mode = false;
    volatile bool calibration_done = false;
    bool low_voltage_detected = false;
    static uint8_t speed = 100;
    TickType_t t = 0;
    mqtt_message msg = {"", ""};
    state robot_state = {3, 10, forward};
    int action = 0;


    CY_ISR_PROTO(Button_Interrupt);
    void print_element(const void *element);
    
    #endif

/* [] END OF FILE */

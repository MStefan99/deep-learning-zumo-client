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


    // Program states
    const int led_timings[][12] = { // Even numbers - ON duration, odd - OFF duration
        {50, 1450, 0}, // State 0 - idle (booted)
        {100, 900, 0},  // State 1 - idle (calibrated)
        {100, 400, 100, 900, 0},  // State 2 - pre-start scanning mode
        {900, 100, 200, 1300}, // State 3 - waiting for server
        {100, 400, 100, 400, 100, 1400, 0},  // State 4 - server-guided navigation mode
        {900, 100, 0},  // State 5 - idle (finished)
        {1300, 50, 100, 50, 0}, // State 6 - motor locked
        {100, 100, 0} // State 7 - error
        };
    volatile bool calibration_mode = false;
    volatile bool calibration_done = false;
    volatile int led_state[2] = {0, 0};
    bool low_voltage_detected = false;
    bool pre_scan = false;
    static uint8_t speed = 100;
    TickType_t t = 0;
    mqtt_message msg = {"", ""};
    state robot_state = {3, 10, forward};
    int action = 0;
    
    
    CY_ISR_PROTO(button_isr);
    CY_ISR_PROTO(led_isr);
    void print_element(const void *element);
    
    #endif

/* [] END OF FILE */

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
    #include "stat.h"
    #include "scan.h"

    
    typedef struct {
        char name[32];
        int movement_enabled;
        int led_timings[10];
    } state;
    
    
    state states[8] = {
        {"Boot idle", 0, {50, 1450, 0}}, // State 0 - idle (booted)
        {"Clb idle", 0, {50, 150, 50, 1250, 0}},  // State 1 - idle (calibrated)
        {"Pre-scan", 1, {300, 1200, 0}},  // State 2 - pre-start scanning mode
        {"Wait", 0, {900, 100, 100, 100, 800, 0}},  // State 3 - waiting for server
        {"Nav", 1, {300, 200, 300, 700, 0}},  // State 4 - server-guided navigation mode
        {"Finish idle", 0, {50, 150, 50, 150, 50, 1050, 0}},  // State 5 - idle (finished)
        {"Locked", 0, {300, 50, 100, 50, 0}},  // State 6 - motor locked
        {"Error", 0, {125, 125, 0}}  // State 7 - error
    };
    volatile int current_state = 7;
    volatile int prev_state = 7;
    volatile int led_state = 0;
    bool calibrated = false;
    bool low_voltage_detected = false;
    static uint8_t speed = 100;
    tile t;
    mqtt_message msg = {"", ""};
    int action = 0;
    
    
    CY_ISR_PROTO(button_isr);
    CY_ISR_PROTO(led_isr);
    void print_element(const void *element);
    void voltage_check();
    void change_state(int state);
    
    #endif

/* [] END OF FILE */

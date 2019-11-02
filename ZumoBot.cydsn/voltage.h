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

#ifndef VOLTAGE_H_
    #define VOLTAGE_H_
    
    #include <project.h>
    #include <stdbool.h>
    #include "states.h"
    #include "smqtt.h"
    
    
    extern bool low_voltage_detected;
    
    
    float battery_voltage();
    int voltage_test();
    void voltage_task();
    
    #endif

/* [] END OF FILE */

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

#ifndef VL53L0X_H
    #define VL53L0X_H
    
    #include "I2C_2.h"
    #include "vl53l0x_api.h"
    #include "vl53l0x_platform.h"
    
    
    int vl53l0x_init();
    int vl53l0x_measure();
    
    void vl53l0x_check();
    
    #endif

/* [] END OF FILE */

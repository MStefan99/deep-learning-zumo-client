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

#ifndef L3GD20H_H
    #define L3GD20H_H
    
    #include "project.h"
    #include <stdio.h>
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "I2C_Common.h"

    typedef struct {
        double x;
        double y;
        double z;
    } gyro_data;
    
    
    int L3GD20H_init();
    int L3GD20H_read(gyro_data *data);
    int L3GD20H_reset();
    int L3GD20H_calibrate();
    
    void GyroQueueInit();
    void L3GD20H_Task();
    
#endif

/* [] END OF FILE */

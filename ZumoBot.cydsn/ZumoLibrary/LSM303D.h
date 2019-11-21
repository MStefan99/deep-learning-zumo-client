/**
 * @file    LSM303D.h
 * @brief   LSM303D header file
 * @details Contains register address definitions, function declarations, and data struct for the LSM303 Accelerometer
*/

#ifndef LSM303D_H_
    #define LSM303D_H_
    
    #include "project.h"
    #include <stdint.h>
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "I2C.h"
    #include "I2C_Common.h"
      
    typedef struct {
        double x;
        double y;
        double z;
    } accelerometer_data;
    
    
    int LSM303D_init();
    int LSM303D_read_acc(accelerometer_data *data);
    int LSM303D_read_mag(accelerometer_data *data);
    int LSM303D_reset();
    int LSM303D_calibrate();
    
    // Do not use
    void LSM303D_task();
#endif
/* [] END OF FILE */

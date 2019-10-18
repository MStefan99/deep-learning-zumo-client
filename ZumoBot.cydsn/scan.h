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


#ifndef SCAN_H_
#define SCAN_H_
    
    #include "movement.h"
    #include "Ultra.h"
    #include "FreeRTOS.h"
    #include "task.h"
    #include "stat.h"
    #include "smqtt.h"
    #include "Beep.h"
    
    #define SOUND_ENABLED 1
    
    typedef struct {
        int x;
        int y;
    } tile;
    
    extern const int tile_size;  // TODO: check
    extern position robot_position;
    
    
    void pre_scan(uint8_t speed);
    tile scan();
    void send_obstacle(tile t);
    void measure_distance(double *dist, double *std_deviation, int count);
    
#endif


/* [] END OF FILE */

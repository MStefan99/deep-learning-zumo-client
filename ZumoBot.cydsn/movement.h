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

#ifndef MOVEMENT_H
#define MOVEMENT_H
    #include <Motor.h>
    #include <stdlib.h>
    #include <FreeRTOS.h>
    #include <task.h>
    #include <stdint.h>
    #include "line_detection.h"
    
    
    typedef enum {
        forward,
        right,
        back, 
        left
    } direction;
    
    
    typedef struct {
        int x;
        int y;
        int direction;
    } state;
    
    
    extern const float p_coefficient;
    extern const float d_coefficient;
    extern state robot_state;
    
    
    void motor_tank_turn(direction dir, uint8_t speed, float delay);
    void motor_turn_diff(uint8_t speed, int diff);
    void move_to_next_intersection(uint8_t speed);
    void rotate(direction dir, uint8_t speed);
    int motor_enabled();
    void set_motor_state(int state);

#endif

/* [] END OF FILE */

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
        
    #include <FreeRTOS.h>
    #include <task.h>
    #include "line_detection.h"
    #include "smqtt.h"
    #include "L3GD20H.h"
    #include "math.h"
    
    #define MOVEMENT_ENABLED 1  // Can be disabled for debugging, does not block or affect program flow
    
    typedef struct {
        int x;
        int y;
        int dir;
    } position;
    
    extern const float p_coefficient;
    extern position robot_position;
    
    
    // Blocking functions
    void move_to_next(uint8_t speed);
    void rotate_to(int dir, uint8_t speed);
    void complete_track(uint8_t speed);
    
    // Non-blocking functions
    void send_coords();
    void motor_tank_turn(int side, uint8_t speed);
    void motor_turn_diff(uint8_t speed, int diff);
    void motor_rotate(int side, uint8_t speed);
    
    // Direct motor control
    int motor_enabled();
    void motor_reset();
    void set_motor_state(int state);

#endif

/* [] END OF FILE */

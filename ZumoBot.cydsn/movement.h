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
        #include "smqtt.h"
    
        #define MOVEMENT_ENABLED 1  // Used for debugging, does not block or affect program flow
        
        typedef struct {
            int x;
            int y;
            int dir;
        } position;
        
        extern const float p_coefficient;
        extern position robot_position;
        
        // Blocking calls
        void move_to_next_intersection(uint8_t speed);
        void rotate_and_center(int dir, uint8_t speed);
        
        // Non-blocking calls
        void send_coords();
        void motor_tank_turn(int side, uint8_t speed);
        void motor_turn_diff(uint8_t speed, int diff);
        void motor_rotate_next(int side, uint8_t speed);
        
        // Motor control
        int motor_enabled();
        void motor_reset();
        void set_motor_state(int state);

    #endif

/* [] END OF FILE */

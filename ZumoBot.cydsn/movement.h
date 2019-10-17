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
            backward,
            left
        } direction;
        
        typedef struct {
            int x;
            int y;
            direction dir;
        } state;
        
        extern const float p_coefficient;
        
        // Blocking calls
        void move_to_next_intersection(state *robot_state, uint8_t speed);
        void rotate(state *robot_state, direction dir, uint8_t speed);
        
        // Non-blocking calls
        void motor_tank_turn(int side, uint8_t speed);
        void motor_turn_diff(uint8_t speed, int diff);
        void rotate_next(int side, uint8_t speed);
        
        // Motor control
        int motor_enabled();
        void reset();
        void set_motor_state(int state);

    #endif

/* [] END OF FILE */

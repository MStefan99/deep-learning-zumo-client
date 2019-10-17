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

#include "movement.h"

const float p_coefficient = 1.5;


void motor_tank_turn(int side, uint8_t speed) {
    MotorDirLeft_Write(!side);
    MotorDirRight_Write(side);
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed);
}


void motor_reset() {
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
    PWM_WriteCompare1(0); 
    PWM_WriteCompare2(0);
}


void motor_turn_diff(uint8_t speed, int diff) {
    uint8_t l_speed = speed;
    uint8_t r_speed = speed;
    if (abs(diff) > speed) {
        if(diff>0) {
            r_speed=0;
        } else {
            l_speed=0;
        }
    } else {
        if (diff > 0){
           r_speed -= diff*speed/255;
       } else {
           l_speed += diff*speed/255;
       }
    }
    PWM_WriteCompare1(l_speed); 
    PWM_WriteCompare2(r_speed);
}


void move_to_next_intersection(state *robot_state, uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        int shift_correction;
        
        while (!intersection_detected()) {
            shift_correction = get_line_pos() * p_coefficient;
            motor_turn_diff(speed, shift_correction);
        }
        
        while (intersection_detected()) {
            PWM_WriteCompare1(speed);
            PWM_WriteCompare2(speed);
        }
        PWM_WriteCompare1(speed);
        PWM_WriteCompare2(speed);
        vTaskDelay(50);
        motor_reset();
        
        if (robot_state->dir % 2) {
            if (robot_state->dir / 2) {
                --robot_state->x;
            } else {
                ++robot_state->x;
            }
        } else {
            if (robot_state->dir / 2) {
                ++robot_state->y;
            } else {
                --robot_state->y;
            }
        }
        mqtt_print("Zumo/Pos", "(%i, %i)", robot_state->x, robot_state->y);
    }
}


int motor_enabled() {
    return PWM_ReadControlRegister() & 0x80;
}


void rotate_next(int side, uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        motor_tank_turn(side, speed);
        vTaskDelay(100);
        
        for (int i = 0; i < 2; ++i) {
            while (!line_centered()) {
                motor_tank_turn(side, speed);
            }
            while (line_centered()) {
                motor_tank_turn(side, speed);
            }
        }
        motor_reset();
    }
}


void rotate(state *robot_state, direction dir, uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        int n = (dir % 2) ^ (robot_state->dir % 2);
        if (!n && dir != robot_state->dir) {
            n = 2;
        }
        int side = dir - robot_state->dir;
        if (dir == 0 && robot_state->dir == 3) {
            side = 1;
        } else if (dir == 3 && robot_state->dir == 0) {
            side = 0;
        }
        if (side < 0) {
            side = 0;
        } else if (side > 1) {
            side = 1;
        }
        for (int i = 0; i < n; ++i) {
            rotate_next(side, speed);
        }
        robot_state->dir = dir;
    }
}


void set_motor_state(int state) {
    if (state) {
        PWM_Start();
    } else {
        PWM_Stop();
    }
    
    BatteryLed_Write(state);
}


/* [] END OF FILE */

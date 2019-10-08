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

const float p_coefficient = 2.5;
const float d_coefficient = 4;
state robot_state = {0, 0, forward};


void motor_tank_turn(direction dir, uint8_t speed, float delay) {
    MotorDirLeft_Write(dir > 2);
    MotorDirRight_Write(dir < 2);
    PWM_WriteCompare1(speed); 
    PWM_WriteCompare2(speed);
    
    vTaskDelay(delay);
    
    MotorDirLeft_Write(0);
    MotorDirRight_Write(0);
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


void move_to_next_intersection(uint8_t speed) {
    int shift_correction;
    if (motor_enabled()) {
        while (!intersection_detected()) {
            shift_correction = get_line_pos() * p_coefficient + get_line_pos_change() * d_coefficient;
            motor_turn_diff(speed, shift_correction);
        }
        
        while (intersection_detected()) {
            PWM_WriteCompare1(speed); 
            PWM_WriteCompare2(speed);
        }
        
        PWM_WriteCompare1(0);
        PWM_WriteCompare2(0);
    }
}


int motor_enabled() {
    return PWM_ReadControlRegister() & 0x80;
}


void rotate(direction dir, uint8_t speed) {
    if (motor_enabled()) {
        PWM_WriteCompare1(speed); 
        PWM_WriteCompare2(speed);
        vTaskDelay(100);
        motor_tank_turn(dir, speed, 500);
        while (!line_centered()) {
            motor_tank_turn(dir, speed, 1);
        }
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

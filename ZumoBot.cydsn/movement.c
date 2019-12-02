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


#define ANGLE 90.0
#define LINE_WIDTH 0.02
#define DIST_TO_CENTER 0.02


const float p_coefficient = 1.0;
position robot_position = {3, 12, 0};


void send_coords() {
    mqtt_print("Ctrl/Zumo/Coords", "(%i, %i)", robot_position.x, robot_position.y);
}


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


void move_to_next(uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        int shift_correction;
        uint32_t t;
        double dt;
        double spd;
        
        while (!intersection_detected()) {
            shift_correction = get_line_pos() * p_coefficient;
            motor_turn_diff(speed, shift_correction);
        }
        
        t = xTaskGetTickCount();
        while (intersection_detected()) {
            PWM_WriteCompare1(speed);
            PWM_WriteCompare2(speed);
        }
        dt = (xTaskGetTickCount() - t) / 1000.0;
        spd = LINE_WIDTH / dt;
        dt = DIST_TO_CENTER / spd;
        
        vTaskDelay((uint32_t)(dt * 1000));
        motor_reset();
    } else {
        vTaskDelay(500);
    }
    
    switch (robot_position.dir) {
        case 0:
            --robot_position.y;
        break;
        
        case 1:
            ++robot_position.x;
        break;
        
        case 2:
            ++robot_position.y;
        break;
        
        case 3:
            --robot_position.x;
        break;
    }
}


int motor_enabled() {
    return PWM_ReadControlRegister() & 0x80;
}


void motor_rotate(int side, uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        gyro_data g = {0, 0, 0};
        
        L3GD20H_reset();
        while (fabs(g.z) < ANGLE / 2.0) {
            L3GD20H_read(&g);
            motor_tank_turn(side, speed);
        }
        while ((!line_centered() || fabs(g.z) < ANGLE * 0.9) && fabs(g.z) < ANGLE) {
            L3GD20H_read(&g);
            motor_tank_turn(side, speed);
        }
        while (!line_detected()) {
            motor_tank_turn(!side, speed);
        }
        motor_reset();
    } else {
        vTaskDelay(500);
    }
}


void rotate_to(int dir, uint8_t speed) {
    if (MOVEMENT_ENABLED) {
        int n = (dir % 2) ^ (robot_position.dir % 2);
        if (!n && dir != robot_position.dir) {
            n = 2;
        }
        int side = dir - robot_position.dir;
        if (dir == 0 && robot_position.dir == 3) {
            side = 1;
        } else if (dir == 3 && robot_position.dir == 0) {
            side = 0;
        }
        if (side < 0) {
            side = 0;
        } else if (side > 1) {
            side = 1;
        }
        
        if ((robot_position.x == 6 && robot_position.dir == 0 && dir == 2) ||  // Right edge
                (robot_position.x == 0 && robot_position.dir == 2 && dir == 0)) {  // Left edge
            side = 0;  // Exceptional case when the robot needs to turn inside on the edges of the track
        }
        for (int i = 0; i < n; ++i) {
            motor_rotate(side, speed);
        }
    } else {
        vTaskDelay(500);
    }
    robot_position.dir = dir;
}


void complete_track(uint8_t speed) {
    rotate_to(0, speed);
    move_to_next(speed);
    while (robot_position.x != 3) {
        if (robot_position.x > 3) {
            rotate_to(3, speed);
            move_to_next(speed);
        } else {
            rotate_to(1, speed);
            move_to_next(speed);
        }
    }
    
    rotate_to(0, speed);
    move_to_next(speed);
    move_to_next(speed);
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

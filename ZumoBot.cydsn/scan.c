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

#include "scan.h"


const int tile_size = 150; //mm
const int pos_fix = 100; //mm


void send_obstacle(tile t) {
    if (t.x != -100 && t.y != -100) {
        mqtt_print("Ctrl/Zumo/Obst", "(%i, %i)", t.x, t.y);
    }
}


void pre_scan(uint8_t speed) {
    tile t;

    rotate_to(3, speed);
    for (int i = 0; i < 3; ++i){
        move_to_next(speed);
    }
    send_coords();

    for (int i = 0; i < 6; ++i) {
        rotate_to(0, speed);
        scan(&t);
        send_obstacle(t);
        rotate_to(1, speed);
        move_to_next(speed);
        send_coords();
    }
    rotate_to(0, speed);
    scan(&t);
    send_obstacle(t);
    rotate_to(3, speed);

    for (int i = 0; i < 3; ++i){
        move_to_next(speed);
    }
    send_coords();
    rotate_to(0, speed);
}


int scan(tile *t) {
    t->x = -100;
    t->y = -100;
    double dist = 0;
    double std_dev = 0;
    
    measure_distance(&dist, &std_dev, 10);
    int dist_tiles = (dist + (double)pos_fix) / tile_size;
    
    
    if (dist < 0) {
        dist_tiles = -1;
    } else if (!dist_tiles) {
        dist_tiles = 1;
    }

    if (dist_tiles > 0 && dist_tiles <= 6 && std_dev < tile_size / 4) {
        t->x = robot_position.x;
        t->y = robot_position.y;
        
        switch (robot_position.dir) {
            case 0:
                t->y -= dist_tiles;
                break;

            case 1:
                t->x += dist_tiles;
                break;

            case 2:
                t->y += dist_tiles;
                break;

            case 3:
                t->x -= dist_tiles;
                break;
        }
        if (SOUND_ENABLED){
            for (int i = 0; i < dist_tiles; ++i) {
                Beep(50, 50);
                vTaskDelay(150);
            }
            vTaskDelay(3000 - 200 * dist_tiles);
        }
    } else {
        if (SOUND_ENABLED) {
            Beep(300, 50);
            vTaskDelay(1200);
        }
    }
    return dist_tiles;
}


void measure_distance(double *dist, double *std_deviation, int count) {
    double m[count];
    double exp;
    double var;
    double dev;

    for (int i = 0; i < count; ++i) {
        m[i] = vl53l0x_measure();
        vTaskDelay(2);
    }

    exp = expected_value(m, count);
    var = variance(m, exp, count);
    dev = standard_deviation(var);

    *dist = exp;
    *std_deviation = dev;
}

/* [] END OF FILE */

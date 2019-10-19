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


const int tile_size = 20;
const int pos_fix = 5;


void send_obstacle(tile t) {
    if (t.x != -100 && t.y != -100) {
        mqtt_print("Zumo/Obst", "(%i, %i)", t.x, t.y);
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
        t = scan();
        send_obstacle(t);
        rotate_to(1, speed);
        move_to_next(speed);
        send_coords();
    }
    rotate_to(0, speed);
    t = scan();
    send_obstacle(t);
    rotate_to(3, speed);

    for (int i = 0; i < 3; ++i){
        move_to_next(speed);
    }
    send_coords();
    rotate_to(0, speed);
}


tile scan() {
    tile t = {robot_position.x, robot_position.y};
    double dist = 0;
    double std_dev = 0;
    measure_distance(&dist, &std_dev, 5);

    int dist_tiles = (dist - (double)pos_fix) / tile_size;
    
    if (!dist_tiles) {
        dist_tiles = 1;
    }

    if (dist_tiles < 5 && std_dev < tile_size / 4) {
        switch (robot_position.dir) {
            case 0:
                t.y -= dist_tiles;
                break;

            case 1:
                t.x += dist_tiles;
                break;

            case 2:
                t.y += dist_tiles;
                break;

            case 3:
                t.x -= dist_tiles;
                break;
        }

        if (SOUND_ENABLED){
            for (int i = 0; i < dist_tiles; ++i) {
                Beep(50, 50);
                vTaskDelay(150);
            }
            vTaskDelay(1500 - 200 * dist_tiles);
        }
    } else {
        t.x = -100;
        t.y = -100;
        if (SOUND_ENABLED) {
            Beep(300, 50);
            vTaskDelay(1200);
        }
    }
    return t;
}


void measure_distance(double *dist, double *std_deviation, int count) {
    double m[count];
    float exp;
    float var;
    float dev;

    for (int i = 0; i < count; ++i) {
        m[i] = (double)Ultra_GetDistance();
        vTaskDelay(2);
    }

    exp = expected_value(m, count);
    var = variance(m, exp, count);
    dev = standard_deviation(var);

    *dist = exp;
    *std_deviation = dev;
}

/* [] END OF FILE */

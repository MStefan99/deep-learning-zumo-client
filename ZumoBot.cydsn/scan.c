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


void send_obstacle(tile t) {
    if (t.x && t.y) {
        mqtt_print("Zumo/Obst", "(%i, %i)", t.x, t.y);
    }
}


void pre_scan(uint8_t speed) {
    tile t;
    
    rotate_and_center(3, speed);
    for (int i = 0; i < 3; ++i){
        move_to_next_intersection(speed);
    }
    send_coords();
    
    for (int i = 0; i < 6; ++i) {
        rotate_and_center(0, speed); 
        t = scan();
        send_obstacle(t);
        rotate_and_center(1, speed);
        move_to_next_intersection(speed);
        send_coords();
    }
    rotate_and_center(0, speed); 
    t = scan();
    send_obstacle(t);
    rotate_and_center(3, speed);
    
    for (int i = 0; i < 3; ++i){
        move_to_next_intersection(speed);
    }
    send_coords();
    rotate_and_center(0, speed);
}


tile scan() {
    tile t = {robot_state.x, robot_state.y};
    double dist = 0;
    double std_dev = 0;
    measure_distance(&dist, &std_dev, 5);
    
    int dist_tiles = dist / tile_size;
    
    
    if (dist_tiles > 0 && dist_tiles < 5 && std_dev < tile_size / 4) {
        switch (robot_state.dir) {
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
    } else {
        t.x = 0;
        t.y = 0;
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

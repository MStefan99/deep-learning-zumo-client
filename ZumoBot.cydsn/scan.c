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


tile scan() {
    tile t = {robot_state.x, robot_state.y};
    int dist = Ultra_GetDistance();
    int dist_tiles = dist / tile_size;
    
    
    if (dist_tiles < 4) {
        switch (robot_state.dir) {
            case left:
                t.x -= dist_tiles;
                break;
            
            case forward:
                t.y -= dist_tiles;
                break;
            
            case right:
                t.x += dist_tiles;
                break;
            
            case backward:
                t.y += dist_tiles;
                break;
        } 
    } else {
        t.x = -1;
        t.y = -1;
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
        vTaskDelay(10);
    }
    
    exp = expected_value(m, count);
    var = variance(m, exp, count);
    dev = standard_deviation(var);
    
    *dist = exp;
    *std_deviation = dev;    
}

/* [] END OF FILE */

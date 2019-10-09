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


/* [] END OF FILE */

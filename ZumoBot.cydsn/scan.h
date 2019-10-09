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


#ifndef SCAN_H_
#define SCAN_H_
    #include "movement.h"
    #include "Ultra.h"
    
    
    const int tile_size = 20;  // TODO: check
    
    typedef struct {
        int x;
        int y;
    } tile;
    
    
    extern state robot_state;
    
    
    tile scan();
    
#endif


/* [] END OF FILE */

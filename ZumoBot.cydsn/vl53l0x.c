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

#include "vl53l0x.h"
#include <stdio.h>


void vl53l0x_init() {
    I2C_2_Start();
    
    printf("Reference registers:\n");
    uint32_t ref;
    VL53L0X_read_multi(0x52, 0xC0, (uint8_t *)&ref, 3);
}


int vl53l0x_measure() {
    return 0;
}

/* [] END OF FILE */

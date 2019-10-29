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

#include "vl53l0x_i2c_platform.h"
#include "I2C.h"
#include "I2C_Common.h"
#include <stdio.h>


int VL53L0x_i2c_init() {
  I2C_2_Start();
  return VL53L0X_ERROR_NONE;
}


int32_t VL53L0X_write_multi(uint8_t address, uint8_t index, uint8_t  *pdata, int32_t count) {
    I2C_2_Write_Multiple(address >> 1, index, pdata, count);
    return 0;
}


int32_t VL53L0X_read_multi(uint8_t address, uint8_t index, uint8_t  *pdata, int32_t count) {
    I2C_2_Read_Multiple(address >> 1, index, pdata, count);
    return 0;
}


int32_t VL53L0X_write_byte(uint8_t address, uint8_t index, uint8_t data) {
    I2C_2_Write_Multiple(address >> 1, index, (uint8_t *)&data, 1);
    return 0;
}


int32_t VL53L0X_write_word(uint8_t address, uint8_t index, uint16_t data) {
    I2C_2_Write_Multiple(address >> 1, index, (uint8_t *)&data, 2);
    return 0;
}


int32_t VL53L0X_write_dword(uint8_t address, uint8_t index, uint32_t data) {
    I2C_2_Write_Multiple(address >> 1, index, (uint8_t *)&data, 4);
    return 0;
}


int32_t VL53L0X_read_byte(uint8_t address, uint8_t index, uint8_t *pdata) {
    I2C_2_Read_Multiple(address >> 1, index, pdata, 1);
    return 0;
}


int32_t VL53L0X_read_word(uint8_t address, uint8_t index, uint16_t *pdata) {
    I2C_2_Read_Multiple(address >> 1, index, (uint8_t *)pdata, 2);
    return 0;
}


int32_t VL53L0X_read_dword(uint8_t address, uint8_t index, uint32_t *pdata) {
    I2C_2_Read_Multiple(address >> 1, index, (uint8_t *)pdata, 4);
    return 0;
}

/* [] END OF FILE */

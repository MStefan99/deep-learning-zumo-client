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

#include "L3GD20H.h"

// Device i2c address
#define L3GD20H 0x6B
#define WHO_AM_I_REF 0xD7

// Register addresses
#define WHO_AM_I 0x0F

#define CTRL1 0x20
#define CTRL2 0x21
#define CTRL3 0x22
#define CTRL4 0x23
#define CTRL5 0x24
#define REFERENCE 0x25
#define OUT_TEMP 0x26
#define STATUS 0x27

#define OUT_X_L 0x28
#define OUT_X_H 0x29
#define OUT_Y_L 0x2A
#define OUT_Y_H 0x2B
#define OUT_Z_L 0x2C
#define OUT_Z_H 0x2D

#define FIFO_CTRL 0x2E
#define FIFO_SRC 0x2F

#define IG_CFG 0x30
#define IG_SRC 0x31
#define IG_THS_XH 0x32
#define IG_THS_XL 0x33
#define IG_THS_YH 0x34
#define IG_THS_YL 0x35
#define IG_THS_ZH 0x36
#define IG_THS_ZL 0x37
#define IG_DURATION 0x38

#define LOW_ODR 0x39

#define READ_MULTI 0x80


QueueHandle_t gyro_out;
QueueHandle_t gyro_ctrl;


void GyroQueueInit() {
    gyro_out = xQueueCreate(1, sizeof(double) * 3);
    gyro_ctrl = xQueueCreate(1, sizeof(uint8_t));
}


int L3GD20H_init() {
    I2C_Start();
    
    uint8_t wai;
    int status = 0;
    status = I2C_Read(L3GD20H, WHO_AM_I, &wai);
    
    if (wai != WHO_AM_I_REF) {
        return -1;
    }
    
    // Beginning startup sequence
    
    status = I2C_Write(L3GD20H, CTRL2, 0x00);  // Setting high pass filter settings
    status = I2C_Write(L3GD20H, CTRL3, 0x00);  // Setting interrupts off
    status = I2C_Write(L3GD20H, CTRL4, 0x40);  // Setting big-endian mode
    status = I2C_Write(L3GD20H, REFERENCE, 0x00);  // Setting high pass filter reference
    
    // Interrupts should be set up by writing to IG_THS, IG_DURATION and IG_CFG but the pin is not connected
    
    status = I2C_Write(L3GD20H, FIFO_CTRL, 0x40);  // Setting Stream mode, threshold 0
    status = I2C_Write(L3GD20H, FIFO_SRC, 0xE0);  // Setting FIFO filling if > threshold, 
                                                  // overrun bit = 1 if completely filed, empty bit = 1 if empty
    status = I2C_Write(L3GD20H, CTRL5, 0x40);  // FIFO enabled, high pass filter off
    status = I2C_Write(L3GD20H, CTRL1, 0x6F);  // Setting 200Hz data rate, active mode, all axes enabled
    
    // Startup sequence is now complete
    
    return status;
}


int L3GD20H_read(gyro_data *data) {
    return xQueueReceive(gyro_out, data, 0);
}


int L3GD20H_calibrate() {
    uint8_t ctrl = 0;
    xQueueReceive(gyro_ctrl, &ctrl, 0);
    ctrl |= 0x03;  // Setting calibration and reset bits
    return xQueueSendToBack(gyro_ctrl, &ctrl, 0);
}


int L3GD20H_reset() {
    uint8_t ctrl = 0;
    xQueueReceive(gyro_ctrl, &ctrl, 0);
    ctrl |= 0x01;  // Setting reset bit
    return xQueueSendToBack(gyro_ctrl, &ctrl, 0);
}


void L3GD20H_Task() {
    L3GD20H_init();
    
    uint8_t tmp[6] = {0};
    uint8_t status_reg;
    uint8_t ctrl = 0;
    gyro_data offset = {0, 0, 0};
    double step_size = 0.00875; // for ±245 dps range sensitivity is 8.75 mdps per digit
    
    gyro_data angle = {0, 0, 0};
    int16_t raw[3] = {0};
    
    uint32_t delay = 10;
    
    while (1) {
        //printf("Gyro task delay %i\n", delay);
        vTaskDelay(delay);
        xQueueReceive(gyro_ctrl, &ctrl, 0);
        
        I2C_Read_Multiple(L3GD20H, STATUS, &status_reg, 1);
        if (!(status_reg & 0x08)) {
            ++delay;  // Data not ready, output undefined, polling rate should be decreased
            continue;  // Skipping data reading
        }
        
        for (int i = 0; i < 32; ++i) {
            I2C_Read_Multiple(L3GD20H, OUT_X_L | READ_MULTI, tmp, 6);
            raw[0] = (tmp[0] << 8 | tmp[1]);
            raw[1] = (tmp[2] << 8 | tmp[3]);
            raw[2] = (tmp[4] << 8 | tmp[5]);
            
            angle.gyro_x += (double) raw[0] * step_size - offset.gyro_x;
            angle.gyro_y += (double) raw[1] * step_size - offset.gyro_y;
            angle.gyro_z += (double) raw[2] * step_size - offset.gyro_z;
        }
        
        if (ctrl & 0x02) {  // Checking calibration bit
            raw[0] = (tmp[0] << 8 | tmp[1]);
            raw[1] = (tmp[2] << 8 | tmp[3]);
            raw[2] = (tmp[4] << 8 | tmp[5]);
            
            offset.gyro_x = (double) raw[0] * step_size;
            offset.gyro_y = (double) raw[1] * step_size;
            offset.gyro_z = (double) raw[2] * step_size;
            ctrl &= 0xFD;  // Clearing calibration bit
        }
        
        if (ctrl & 0x01) {  // Checking reset bit
            angle.gyro_x = 0;  // Resetting the position
            angle.gyro_y = 0;
            angle.gyro_z = 0;
            ctrl &= 0xFE;  // Clearing reset bit
        }
        
        xQueueReset(gyro_out);
        xQueueSendToBack(gyro_out, &angle, 0);
        
        if (status_reg & 0x80 && delay > 1) {
            --delay;  // Overwriting data, output ok, polling rate should be increased
        }
    }
}

/* [] END OF FILE */

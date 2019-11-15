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


// Macros for easy setup


// Returns time between samples
#define dt(freq) (1.0 / freq)

// Returns sensor sensitivity for chosen range
#define sensitivity(scale) ( \
    (scale == 245? 0.00875 : \
    scale == 500? 0.0175 : 0.07) \
)

// Do not use!
#define mode(freq) (\
    (freq == 100)? (0x3 << 4): \
    (freq == 200)? (0x6 << 4): \
    (freq == 400)? (0xB << 4): \
    (freq == 800)? (0xE << 4): \
    (0x3 << 4) \
) // Defaulting to 100 Hz

/* Helps to set up CTRL1 register.
*   freq - sensor sampling frequency. (100, 200, 400 or 800Hz)
*   power_on - enables active mode (0 - sleep mode, 1 - active mode)
*   last 3 parameters are used to individually activate 3 sensor axes: x, y and z (0 - off, 1 - on)
*/
#define ctrl1(freq, power_on, x_enable, y_enable, z_enable) (\
    (mode(freq)) | \
    (power_on? (0x1 << 3) : (0x0)) | \
    (x_enable? (0x1 << 1) : (0x0)) | \
    (y_enable? (0x1) : (0x0)) | \
    (z_enable? (0x1 << 2) : (0x0)) \
)

/* Helps to set up CTRL4 register.
*   block_data_update - block data update. 
*       (0 - continuos update; 1 - output registers not updated until MSB and LSB reading)
*   big_endian - big/little endian data selection. (0 - Little endian, 1 - Big endian)
*   scale - scale selection. (245, 500 or 2000dps)
*/
#define ctrl4(block_data_update, big_endian, scale) ( \
    (block_data_update? (0x1 << 7) : 0) | \
    (big_endian? (0x1 << 6) : 0) | \
    (((scale == 245)? 0x0 : \
    (scale == 500)? 0x1 : 0x2) << 4) \
)

/* Helps to set up CTRL5 register.
*   reboot_memory - Reboot memory content.
*       (0 - normal mode; 1 - reboot memory content)
*   FIFO_enable - enables FIFO buffer. (0 - FIFO off, 1 - FIFO on)
*   FIFO_limit_enable - stops FIFO filling at threshold (0 - limit off, 1 - limit on)
*   high_pass_enable - enables high pass filter (0 - filter off, 1 - filter on)
*/
#define ctrl5(reboot_memory, FIFO_enable, FIFO_limit_enable, high_pass_enable) ( \
    (reboot_memory? (0x1 << 7) : 0) | \
    (FIFO_enable? (0x1 << 6) : 0) | \
    (FIFO_limit_enable? (0x1 << 5) : 0) | \
    (high_pass_enable? (0x1 << 4) : 0) \
)

/* Helps to set up CTRL5 register.
*   mode - sets FIFO mode (modes 1-7)
*   threshsold - number of FIFO registers to be filled to enable threshold status (0-32 registers)
*/
#define fifo_ctrl(mode, threshold) ( \
    (mode << 5) | \
    (threshold) \
)

#define data_overrun(status_reg) ( \
    (status_reg & 0x80) \
)

#define data_available(status_reg) ( \
    (status_reg & 0x8) \
)

#define fifo_threshold(fifo_src_reg) ( \
    (fifo_src_reg & 0x80)? 1 : 0 \
)

#define fifo_overrun(fifo_src_reg) ( \
    (fifo_src_reg & 0x40)? 1 : 0 \
)

#define fifo_empty(fifo_src_reg) ( \
    (fifo_src_reg & 0x20)? 1 : 0 \
)

#define fifo_unread_num(fifo_src_reg) ( \
    (fifo_src_reg & 0x1F) \
)

#define get_angle(out_acc_h, out_acc_l, freq, scale) ( \
    (int16_t)(out_acc_h << 8 | out_acc_l) * dt(freq) * sensitivity(scale) \
)
    
// End of setup macros


#define frequency 800
#define range 245
#define FIFO_READ 0x80


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
    
    status = I2C_Write(L3GD20H, CTRL4, ctrl4(0, 1, range));  // Setting big-endian mode, range +-245
    status = I2C_Write(L3GD20H, REFERENCE, 0);  // Setting high pass filter reference
    
    // Interrupts should be set up here by writing to IG_THS, IG_DURATION and IG_CFG but interrupt pin isn't connected
    
    status = I2C_Write(L3GD20H, FIFO_CTRL, fifo_ctrl(2, 30));  // Setting Stream mode, threshold 30
    status = I2C_Write(L3GD20H, CTRL5, ctrl5(0, 1, 0, 0));  // FIFO enabled, depth not limited, 
                                                            //high pass filter disabled
    status = I2C_Write(L3GD20H, CTRL1, ctrl1(frequency, 1, 0, 0, 1));  // Setting 200Hz data rate, 
                                                                       // active mode, only x axis enabled
    
    // Startup sequence is now complete
    return status;
}


int L3GD20H_read(gyro_data *data) {
    return xQueueReceive(gyro_out, data, 0);
}


int L3GD20H_calibrate() {
    uint8_t ctrl = 0;
    xQueueReceive(gyro_ctrl, &ctrl, 0);
    ctrl |= 0x02;  // Setting calibration and reset bits
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
    
    uint8_t tmp[2] = {0};
    uint8_t status_reg = 0x0;
    uint8_t task_ctrl = 0x0;
    uint8_t fifo_status = 0x0;
    
    gyro_data offset = {0, 0, 0};
    gyro_data gyro = {0, 0, 0};
    double angle_batch = 0;
    double angle = 0;
    
    uint32_t delay = 100;
    
    while (1) {
        vTaskDelay(delay);
        xQueueReceive(gyro_ctrl, &task_ctrl, 0);
        angle_batch = 0;
        
        I2C_Read(L3GD20H, FIFO_SRC, &fifo_status);
        if (!fifo_threshold(fifo_status)) {
            ++delay;  // FIFO not filled completely, we can decrease polling rate to reduce load
            
            if (fifo_empty(fifo_status)) {
                delay += 5;  // FIFO empty, output undefined, polling rate should be decreased
                continue;  // Skipping data reading
            }
        } else {
            --delay;  // FIFO almost filled, polling rate should be increased 
        }
        
        for (int i = 0; i < fifo_unread_num(fifo_status); ++i) {
            I2C_Read_Multiple(L3GD20H, OUT_Z_L | FIFO_READ, tmp, 2);
            angle = get_angle(tmp[0], tmp[1], frequency, range);
            
            angle_batch += angle;
            gyro.z += (double) angle - offset.z;
        }
        
        if (task_ctrl & 0x02) {  // Checking calibration bit
            offset.z = angle_batch / fifo_unread_num(fifo_status);
            
            task_ctrl &= 0xFD;  // Clearing calibration bit
        }
        
        if (task_ctrl & 0x01) {  // Checking reset bit
            gyro.z = 0;  // Resetting the position
            
            task_ctrl &= 0xFE;  // Clearing reset bit
        }
        
        xQueueReset(gyro_out);
        xQueueSendToBack(gyro_out, &gyro, 0);
        
        if (fifo_overrun(status_reg) && delay > 1) {
            delay -=5;  // Overwriting data, output ok, polling rate should be increased
        }
    }
}

/* [] END OF FILE */

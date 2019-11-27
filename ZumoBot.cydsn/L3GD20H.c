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

#define L3GD20H                 0x6B
#define L3GD20H_WHO_AM_I_REF    0xD7


// Register addresses

#define L3GD20H_WHO_AM_I    0x0F

#define L3GD20H_CTRL1       0x20
#define L3GD20H_CTRL2       0x21
#define L3GD20H_CTRL3       0x22
#define L3GD20H_CTRL4       0x23
#define L3GD20H_CTRL5       0x24
#define L3GD20H_REFERENCE   0x25
#define L3GD20H_OUT_TEMP    0x26
#define L3GD20H_STATUS      0x27

#define L3GD20H_OUT_X_L     0x28
#define L3GD20H_OUT_X_H     0x29
#define L3GD20H_OUT_Y_L     0x2A
#define L3GD20H_OUT_Y_H     0x2B
#define L3GD20H_OUT_Z_L     0x2C
#define L3GD20H_OUT_Z_H     0x2D

#define L3GD20H_FIFO_CTRL   0x2E
#define L3GD20H_FIFO_SRC    0x2F

#define L3GD20H_IG_CFG      0x30
#define L3GD20H_IG_SRC      0x31
#define L3GD20H_IG_THS_XH   0x32
#define L3GD20H_IG_THS_XL   0x33
#define L3GD20H_IG_THS_YH   0x34
#define L3GD20H_IG_THS_YL   0x35
#define L3GD20H_IG_THS_ZH   0x36
#define L3GD20H_IG_THS_ZL   0x37
#define L3GD20H_IG_DURATION 0x38

#define L3GD20H_LOW_ODR     0x39


// Macros for easy setup


// Returns time between samples
#define L3GD20H_dt(freq) (1.0 / L3GD20H_real_data_rate(freq))

// Returns sensor sensitivity for chosen range
#define L3GD20H_sensitivity(scale) ( \
    (scale == 245? 0.00875 : \
    scale == 500? 0.0175 : 0.07) \
)

// Do not use!
#define L3GD20H_odr(freq) (\
    (freq == 100)? (0x3 << 4): \
    (freq == 200)? (0x6 << 4): \
    (freq == 400)? (0xB << 4): \
    (freq == 800)? (0xE << 4): \
    (0x3 << 4) \
) // Defaulting to 100 Hz

#define L3GD20H_real_data_rate(freq) (\
    (freq == 100)? (94.7): \
    (freq == 200)? (189.4): \
    (freq == 400)? (378.8): \
    (freq == 800)? (757.6): \
    (94.7) \
) // Defaulting to 100 Hz

/* Helps to set up CTRL1 register.
*   freq - sensor sampling frequency (100, 200, 400 or 800Hz)
*   power_on - enables active mode (0 - sleep mode, 1 - active mode)
*   last 3 parameters are used to individually activate 3 sensor axes: x, y and z (0 - off, 1 - on)
*/
#define L3GD20H_ctrl1(freq, power_on, x_enable, y_enable, z_enable) (\
    (L3GD20H_odr(freq)) | \
    (power_on? (0x1 << 3) : (0x0)) | \
    (x_enable? (0x1 << 1) : (0x0)) | \
    (y_enable? (0x1) : (0x0)) | \
    (z_enable? (0x1 << 2) : (0x0)) \
)

/* Helps to set up CTRL4 register.
*   block_data_update - block data update 
*       (0 - continuos update, 1 - output registers not updated until MSB and LSB reading)
*   big_endian - big/little endian data selection (0 - Little endian, 1 - Big endian)
*   scale - scale selection (+-245, +-500 or +-2000dps)
*/
#define L3GD20H_ctrl4(block_data_update, big_endian, scale) ( \
    (block_data_update? (0x1 << 7) : 0) | \
    (big_endian? (0x1 << 6) : 0) | \
    (((scale == 245)? 0x0 : \
    (scale == 500)? 0x1 : 0x2) << 4) \
)  // Defaulting to biggest range

/* Helps to set up CTRL5 register.
*   reboot_memory - reboot memory content
*       (0 - normal mode, 1 - reboot memory content)
*   FIFO_enable - enables FIFO buffer (0 - FIFO off, 1 - FIFO on)
*   FIFO_limit_enable - stops FIFO filling at threshold (0 - limit off, 1 - limit on)
*   high_pass_enable - enables high pass filter (0 - filter off, 1 - filter on)
*/
#define L3GD20H_ctrl5(reboot_memory, FIFO_enable, FIFO_limit_enable, high_pass_enable) ( \
    (reboot_memory? (0x1 << 7) : 0) | \
    (FIFO_enable? (0x1 << 6) : 0) | \
    (FIFO_limit_enable? (0x1 << 5) : 0) | \
    (high_pass_enable? (0x1 << 4) : 0) \
)

/* Helps to set up FIFO_CTRL register.
*   mode - sets FIFO mode (Available modes 1-7, see datasheet for more info)
*   threshsold - number of FIFO registers to be filled to enable threshold status (0-32 registers)
*/
#define L3GD20H_fifo_ctrl(mode, threshold) ( \
    (mode << 5) | \
    (threshold) \
)

#define L3GD20H_data_overrun(status_reg) ( \
    (status_reg & 0x80) \
)

#define L3GD20H_data_available(status_reg) ( \
    (status_reg & 0x8) \
)

#define L3GD20H_fifo_threshold(fifo_src_reg) ( \
    (fifo_src_reg & 0x80)? 1 : 0 \
)

#define L3GD20H_fifo_overrun(fifo_src_reg) ( \
    (fifo_src_reg & 0x40)? 1 : 0 \
)

#define L3GD20H_fifo_empty(fifo_src_reg) ( \
    (fifo_src_reg & 0x20)? 1 : 0 \
)

#define L3GD20H_fifo_unread_num(fifo_src_reg) ( \
    (fifo_src_reg & 0x1F) \
)

#define L3GD20H_get_angle(out_acc_h, out_acc_l, freq, scale) ( \
    (int16_t)(out_acc_h << 8 | out_acc_l) * L3GD20H_dt(freq) * L3GD20H_sensitivity(scale) \
)
    
// End of setup macros


#define L3GD20H_frequency 800
#define L3GD20H_range 245
#define L3GD20H_FIFO_READ 0x80
#define L3GD20H_FIFO_depth 20 
#define x_enabled 0
#define y_enabled 0
#define z_enabled 1


static gyro_data gyro_out;
QueueHandle_t gyro_ctrl;


void L3GD20H_queue_init() {
    gyro_ctrl = xQueueCreate(1, sizeof(uint8_t));
}


int L3GD20H_init() {
    uint8_t wai;
    int status = 0;
    status = I2C_Read(L3GD20H, L3GD20H_WHO_AM_I, &wai);
    
    if (wai != L3GD20H_WHO_AM_I_REF) {
        return -1;
    }
    // Beginning startup sequence
    
    // Setting big-endian mode, sensor range
    status = I2C_Write(L3GD20H, L3GD20H_CTRL4, L3GD20H_ctrl4(0, 1, L3GD20H_range));  
    // Setting high pass filter reference
    status = I2C_Write(L3GD20H, L3GD20H_REFERENCE, 0);  
    
    // Interrupts should be set up here by writing to IG_THS, IG_DURATION and IG_CFG but interrupt pin isn't connected
    
    // Enabling FIFO in stream mode, setting FIFO threshold
    status = I2C_Write(L3GD20H, L3GD20H_FIFO_CTRL, L3GD20H_fifo_ctrl(2, L3GD20H_FIFO_depth)); 
    // FIFO enabled, depth not limited, high pass filter disabled
    status = I2C_Write(L3GD20H, L3GD20H_CTRL5, L3GD20H_ctrl5(0, 1, 0, 0));  
    // Setting data rate, active mode, enabling axes
    status = I2C_Write(L3GD20H, L3GD20H_CTRL1, L3GD20H_ctrl1(L3GD20H_frequency, 1, x_enabled, y_enabled, z_enabled)); 
    
    // Startup sequence is now complete
    return status;
}


int L3GD20H_read(gyro_data *data) {
    *data = gyro_out;
    return 0;
}


int L3GD20H_calibrate() {
    uint8_t ctrl = 0;
    xQueueReceive(gyro_ctrl, &ctrl, 0);
    ctrl |= 0x02;  // Setting calibration bit
    return xQueueSendToBack(gyro_ctrl, &ctrl, 0);
}


int L3GD20H_reset() {
    uint8_t ctrl = 0;
    xQueueReceive(gyro_ctrl, &ctrl, 0);
    ctrl |= 0x01;  // Setting reset bit
    return xQueueSendToBack(gyro_ctrl, &ctrl, 0);
}


void L3GD20H_task() {
    L3GD20H_init();
    
    uint8_t tmp[2] = {0};
    uint8_t status_reg = 0x0;
    uint8_t task_ctrl = 0x0;
    uint8_t fifo_status = 0x0;
    
    gyro_data offset = {0, 0, 0};
    gyro_data angle_batch = {0, 0, 0};
    gyro_data angle = {0, 0, 0};
    
    uint32_t delay = 100;
    
    while (1) {
        xQueueReceive(gyro_ctrl, &task_ctrl, delay);
        angle_batch.x = 0;
        angle_batch.y = 0;
        angle_batch.z = 0;
        
        I2C_Read(L3GD20H, L3GD20H_FIFO_SRC, &fifo_status);
        if (!L3GD20H_fifo_threshold(fifo_status)) {
            ++delay;  // FIFO not filled completely, we can decrease polling rate to reduce load
            
            if (L3GD20H_fifo_empty(fifo_status)) {
                delay += 5;  // FIFO empty, output undefined, polling rate should be decreased
                continue;  // Skipping data reading
            }
        } else if (delay > 1) {
            --delay;  // FIFO almost filled, polling rate should be increased 
        }
        
        for (int i = 0; i < L3GD20H_fifo_unread_num(fifo_status); ++i) {
            if (x_enabled) {
                I2C_Read_Multiple(L3GD20H, L3GD20H_OUT_X_L | L3GD20H_FIFO_READ, tmp, 2);
                
                angle.x = L3GD20H_get_angle(tmp[0], tmp[1], L3GD20H_frequency, L3GD20H_range);
                
                angle_batch.x += angle.x;
                gyro_out.x += (double) angle.x - offset.x;
            }
            
            if (y_enabled) {
                I2C_Read_Multiple(L3GD20H, L3GD20H_OUT_Y_L | L3GD20H_FIFO_READ, tmp, 2);
                
                angle.y = L3GD20H_get_angle(tmp[0], tmp[1], L3GD20H_frequency, L3GD20H_range);
                
                angle_batch.y += angle.y;
                gyro_out.y += (double) angle.y - offset.y;
            }
            
            if (z_enabled) {
                I2C_Read_Multiple(L3GD20H, L3GD20H_OUT_Z_L | L3GD20H_FIFO_READ, tmp, 2);
                
                angle.z = L3GD20H_get_angle(tmp[0], tmp[1], L3GD20H_frequency, L3GD20H_range);
                
                angle_batch.z += angle.z;
                gyro_out.z += (double) angle.z - offset.z;
            }
        }
        
        if (task_ctrl & 0x02) {  // Checking calibration bit
            offset.x = angle_batch.x / L3GD20H_fifo_unread_num(fifo_status);
            offset.y = angle_batch.y / L3GD20H_fifo_unread_num(fifo_status);
            offset.z = angle_batch.z / L3GD20H_fifo_unread_num(fifo_status);
            
            task_ctrl &= 0xFD;  // Clearing calibration bit
        }
        
        if (task_ctrl & 0x01) {  // Checking reset bit
            gyro_out.x = 0;
            gyro_out.y = 0;  // Resetting the position
            gyro_out.z = 0;
            
            task_ctrl &= 0xFE;  // Clearing reset bit
        }
        
        if (L3GD20H_fifo_overrun(status_reg) && delay > 5) {
            delay -=5;  // Overwriting data, output ok, polling rate should be increased
        }
    }
}

/* [] END OF FILE */

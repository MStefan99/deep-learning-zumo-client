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

#include "LSM303D.h"


// Device i2c address
#define LSM303D                     0x1D
#define LSM303D_WHO_AM_I_REF        0x49

// Register addresses
#define LSM303D_WHO_AM_I            0x0F
    
#define LSM303D_REFERENCE_X         0x1C
#define LSM303D_REFERENCE_Y         0x1D
#define LSM303D_REFERENCE_Z         0x1E

#define LSM303D_CTRL0               0x1F
#define LSM303D_CTRL1               0x20
#define LSM303D_CTRL2               0x21
#define LSM303D_CTRL3               0x22
#define LSM303D_CTRL4               0x23
#define LSM303D_CTRL5               0x24
#define LSM303D_CTRL6               0x25
#define LSM303D_CTRL7               0x26

#define LSM303D_STATUS_M            0x07
#define LSM303D_OUT_X_L_M           0x08
#define LSM303D_OUT_X_H_M           0x09
#define LSM303D_OUT_Y_L_M           0x0A
#define LSM303D_OUT_Y_H_M           0x0B
#define LSM303D_OUT_Z_L_M           0x0C
#define LSM303D_OUT_Z_H_M           0x0D

#define LSM303D_STATUS_A            0x27
#define LSM303D_OUT_X_L_A           0x28
#define LSM303D_OUT_X_H_A           0x29
#define LSM303D_OUT_Y_L_A           0x2A
#define LSM303D_OUT_Y_H_A           0x2B
#define LSM303D_OUT_Z_L_A           0x2C
#define LSM303D_OUT_Z_H_A           0x2D

#define LSM303D_FIFO_CTRL           0x2E
#define LSM303D_FIFO_SRC            0x2F

#define LSM303D_IG_CFG1             0x30
#define LSM303D_IG_SRC1             0x31
#define LSM303D_IG_THS1             0x32
#define LSM303D_IG_DUR1             0x33
#define LSM303D_IG_CFG2             0x34
#define LSM303D_IG_SRC2             0x35
#define LSM303D_IG_THS2             0x36
#define LSM303D_IG_DUR2             0x37

#define LSM303D_CLICK_CFG           0x38
#define LSM303D_CLICK_SRC           0x39
#define LSM303D_CLICK_THS           0x3A
#define LSM303D_TIME_LIMIT          0x3B
#define LSM303D_TIME_LATENCY        0x3C
#define LSM303D_TIME_WINDOW         0x3D

#define LSM303D_ACT_THS             0x3E
#define LSM303D_ACT_DUR             0x3F

#define LSM303D_TEMP_OUT_L          0x05
#define LSM303D_TEMP_OUT_H          0x06

#define G 9.81


// Macros for easy setup


// Returns time between samples
#define LSM303D_dt(freq) (1.0 / freq)

// Returns acceleration sensitivity of the sensor
#define LSM303D_acc_sensitivity(scale) ( \
    (scale == 2)? 0.000061 : \
    (scale == 4)? 0.000122 : \
    (scale == 6)? 0.000183 : \
    (scale == 8)? 0.000244 : \
    0.000734 \
)  // Defaulting to widest range

// Returns magnetic sensitivity of the sensor
#define LSM303D_mag_sensitivity(scale) ( \
    (scale == 2)? 0.00008 : \
    (scale == 4)? 0.00016 : \
    (scale == 8)? 0.00032 : \
    0.000479 \
)  // Defaulting to highest range

// Do not use
#define LSM303D_aodr(freq) ( \
    (freq == 0)? 0 : \
    (freq == 3.125)? (0x0 << 2): \
    (freq == 6.25)? (0x1 << 2): \
    (freq == 12.5)? (0x2 << 2): \
    (freq == 25)? (0x3 << 2): \
    (freq == 50)? (0x05 << 4): \
    (freq == 100)? (0x06 << 4): \
    (freq == 200)? (0x07 << 4): \
    (freq == 400)? (0x08 << 4): \
    (freq == 800)? (0x09 << 4): \
    (freq == 1600)? (0x0A << 4): \
    (0x05 << 4) \
) // Defaulting to 50 Hz

// Do not use
#define LSM303D_modr(freq) ( \
    (freq == 3.125)? (0x0 << 2): \
    (freq == 6.25)? (0x1 << 2): \
    (freq == 12.5)? (0x2 << 2): \
    (freq == 25)? (0x3 << 2): \
    (freq == 50)? (0x4 << 2): \
    (freq == 100)? (0x5 << 2): \
    (0x0 << 2) \
) // Defaulting to 3.125 Hz

/* Helps to set up CTRL0 register.
*   reboot_memory - reboot_memory - Reboot memory content
*       (0 - normal mode, 1 - reboot memory content)
*   FIFO_enable - enables FIFO buffer (0 - FIFO off, 1 - FIFO on)
*   FIFO_limit_enable - stops FIFO filling at threshold (0 - limit off, 1 - limit on)
*/
#define LSM303D_ctrl0(reboot_memory, FIFO_enable, FIFO_limit_enable) ( \
    (reboot_memory? (0x1 << 7) : 0) | \
    (FIFO_enable? (0x1 << 6) : 0) | \
    (FIFO_limit_enable? (0x1 << 5) : 0) \
)

/* Helps to set up CTRL1 register.
*   freq - acceleration sampling frequency (see datasheet or "LSM303D_aodr" for available values)
*   block_data_update - block data update
*       (0 - continuos update, 1 - output registers not updated until MSB and LSB reading)
*   power_on - enables active mode (0 - sleep mode, 1 - active mode)
*   last 3 parameters are used to individually activate 3 sensor axes: x, y and z (0 - off, 1 - on)
*/
#define LSM303D_ctrl1(freq, block_data_update, x_enable, y_enable, z_enable) ( \
    (LSM303D_aodr(freq)) | \
    (block_data_update? (0x1 << 3) : 0) | \
    (x_enable? 0x01 : 0) | \
    (y_enable? (0x01 << 1) : 0) | \
    (z_enable? (0x01 << 2) : 0) \
)

/* Helps to set up CTRL4 register.
*   scale - acceleration scale selection (+-2, +-4, +-6, +-8 or +-16 g)
*/
#define LSM303D_ctrl2(scale) ( \
    (scale == 2)? (0x0 << 3): \
    (scale == 4)? (0x1 << 3): \
    (scale == 6)? (0x2 << 3): \
    (scale == 8)? (0x3 << 3): \
    (0x4 << 3) \
)  // Defaulting to widest range

/* Helps to set up CTRL5 register.
*   temp_enable - enable temperature sensor (0 - off, 1 - off)
*   magnetic_high_resolution - enable high magnetic sensor resolution (0 - low, 1- high)
*   magneetic_freq - magnetic sampling frequency (see datasheet or "LSM303D_modr" for available values)
*/
#define LSM303D_ctrl5(temp_enable, magnetic_high_resolution, magnetic_freq) ( \
    LSM303D_modr(magnetic_freq) | \
    (temp_enable? (0x01 << 7) : 0) | \
    (magnetic_high_resolution? (0x3 << 5) : 0) \
)
    
/* Helps to set up CTRL6 register.
*   scale - magnetic scale selection (+-2, +-4, +-8 or +-12 gauss)
*/
#define LSM303D_ctrl6(scale) ( \
    (scale == 2)? (0x0 << 5): \
    (scale == 4)? (0x1 << 5): \
    (scale == 8)? (0x2 << 5): \
    (scale == 12)? (0x3 << 5): \
    (0x3 << 5) \
)  // Defaulting to widest range
    
/* Helps to set up CTRL7 register.
*   mode - magnetic mode selection (Available modes 0-3, see datasheet for more info)
*/
#define LSM303D_ctrl7(magnetic_mode) ( \
    (magnetic_mode) \
)

/* Helps to set up FIFO_CTRL register.
*   mode - sets FIFO mode (Available modes 1-7, see datasheet for more info)
*   threshsold - number of FIFO registers to be filled to enable threshold status (0-32 registers)
*/
#define LSM303D_fifo_ctrl(fifo_mode, threshold) ( \
    (fifo_mode << 5) | \
    (threshold) \
)

#define LSM303D_fifo_threshold(fifo_src_reg) ( \
    (fifo_src_reg & 0x80) \
)

#define LSM303D_fifo_overrun(fifo_src_reg) ( \
    (fifo_src_reg & 0x40) \
)

#define LSM303D_fifo_empty(fifo_src_reg) ( \
    (fifo_src_reg & 0x20) \
)

#define LSM303D_fifo_unread_num(fifo_src_reg) ( \
    (fifo_src_reg & 0x1F) \
)

#define LSM303D_data_overrun(status_reg) ( \
    (a_status_reg & 0x80) \
)

#define LSM303D_data_available(status_reg) ( \
    (a_status_reg & 0x8) \
)

#define LSM303D_get_acc(out_acc_h, out_acc_l, scale) ( \
    (int16_t)(out_acc_h << 8 | out_acc_l) * LSM303D_acc_sensitivity(scale) * G \
)

#define LSM303D_get_mag(out_acc_h, out_acc_l, scale) ( \
    (int16_t)(out_acc_h << 8 | out_acc_l) * LSM303D_mag_sensitivity(scale) \
)

// End of setup macros

#define LSM303D_acc_range 2
#define LSM303D_acc_frequency 200

#define LSM303D_mag_range 12
#define LSM303D_mag_frequency 25

#define LSM303D_FIFO_READ 0x80
#define LSM303D_FIFO_depth 30
#define x_enabled 1
#define y_enabled 1
#define z_enabled 1

static accelerometer_data acc = {0, 0, 0};
static accelerometer_data mag = {0, 0, 0};


int LSM303D_init() {
    uint8_t wai;
    int status = 0;
    status = I2C_Read(LSM303D, LSM303D_WHO_AM_I, &wai);
    
    if (wai != LSM303D_WHO_AM_I_REF) {
        return -1;
    }
    // Beginning startup sequence
    
    // FIFO enabled, FIFO limit off
    status = I2C_Write(LSM303D, LSM303D_CTRL0, LSM303D_ctrl0(0, 1, 0)); 
    // Setting accelerometer frequency, enabling axes
    status = I2C_Write(LSM303D, LSM303D_CTRL1, LSM303D_ctrl1(LSM303D_acc_frequency, 0, x_enabled, y_enabled, z_enabled));
    // Setting accelerometer range
    status = I2C_Write(LSM303D, LSM303D_CTRL2, LSM303D_ctrl2(LSM303D_acc_range));
    // Setting magnetic frequency, high precision
    status = I2C_Write(LSM303D, LSM303D_CTRL5, LSM303D_ctrl5(0, 1, LSM303D_mag_frequency));
    // Setting magnetic range
    status = I2C_Write(LSM303D, LSM303D_CTRL6, LSM303D_ctrl6(LSM303D_mag_range));
    // Setting continous magnetic mode
    status = I2C_Write(LSM303D, LSM303D_CTRL7, LSM303D_ctrl7(0));
    // Enabling FIFO in stream mode, setting FIFO threshold
    status = I2C_Write(LSM303D, LSM303D_FIFO_CTRL, LSM303D_fifo_ctrl(2, LSM303D_FIFO_depth));
    
    // Startup sequence complete
    return status;
}


int LSM303D_read_acc(accelerometer_data *data) {
    *data = acc;
    return 0;
}


int LSM303D_read_mag(accelerometer_data *data) {
    *data = mag;
    return 0;
}


void LSM303D_task() {
    LSM303D_init();
    
    uint8_t tmp[2] = {0};
    uint8_t status_reg = 0x0;
    uint8_t fifo_status = 0x0;
    
    uint32_t delay = 100;
    
    while (1) {
        vTaskDelay(delay);
        I2C_Read(LSM303D, LSM303D_FIFO_SRC, &fifo_status);
        
        if (!LSM303D_fifo_threshold(fifo_status)) {
            ++delay;  // FIFO not filled completely, we can decrease polling rate to reduce load
            if (LSM303D_fifo_empty(fifo_status)) {
                delay += 5;  // FIFO empty, output undefined, polling rate should be decreased
                continue;  // Skipping data reading
            }
        } else if (delay > 1) {
            --delay;  // FIFO almost filled, polling rate should be increased 
        }
        
        for (int i = 0; i < LSM303D_fifo_unread_num(fifo_status); ++i) {
            if (x_enabled) {
                I2C_Read_Multiple(LSM303D, LSM303D_OUT_X_L_A | LSM303D_FIFO_READ, tmp, 2);
                acc.x = LSM303D_get_acc(tmp[1], tmp[0], LSM303D_acc_range);
            }
            
            if (y_enabled) {
                I2C_Read_Multiple(LSM303D, LSM303D_OUT_Y_L_A | LSM303D_FIFO_READ, tmp, 2);
                acc.y = LSM303D_get_acc(tmp[1], tmp[0], LSM303D_acc_range);
            }
            
            if (z_enabled) {
                I2C_Read_Multiple(LSM303D, LSM303D_OUT_Z_L_A | LSM303D_FIFO_READ, tmp, 2);
                acc.z = LSM303D_get_acc(tmp[1], tmp[0], LSM303D_acc_range);
            }
            
            I2C_Read_Multiple(LSM303D, LSM303D_OUT_X_L_M | LSM303D_FIFO_READ, tmp, 2);
            mag.x = LSM303D_get_mag(tmp[1], tmp[0], LSM303D_acc_range);
            I2C_Read_Multiple(LSM303D, LSM303D_OUT_Y_L_M | LSM303D_FIFO_READ, tmp, 2);
            mag.y = LSM303D_get_mag(tmp[1], tmp[0], LSM303D_acc_range);
            I2C_Read_Multiple(LSM303D, LSM303D_OUT_Z_L_M | LSM303D_FIFO_READ, tmp, 2);
            mag.z = LSM303D_get_mag(tmp[1], tmp[0], LSM303D_acc_range);
        }
             
        if (LSM303D_fifo_overrun(status_reg) && delay > 5) {
            delay -=5;  // Overwriting data, output ok, polling rate should be increased
        }
    }
}

/* [] END OF FILE */

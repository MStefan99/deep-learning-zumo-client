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


// Macros for easy setup

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
    
#define LSM303D_modr(freq) ( \
    (freq == 3.125)? (0x0 << 2): \
    (freq == 6.25)? (0x1 << 2): \
    (freq == 12.5)? (0x2 << 2): \
    (freq == 25)? (0x3 << 2): \
    (freq == 50)? (0x4 << 2): \
    (freq == 100)? (0x5 << 2): \
    (0x0 << 2) \
) // Defaulting to 3.125 Hz

#define LSM303D_ctrl0(reboot_memory, FIFO_enable, FIFO_limit_enable) ( \
    (reboot_memory? (0x1 << 7) : 0) | \
    (FIFO_enable? (0x1 << 6) : 0) | \
    (FIFO_limit_enable? (0x1 << 5) : 0) \
)

#define LSM303D_ctrl1(freq, block_data_update, x_enable, y_enable, z_enable) ( \
    (LSM303D_aodr(freq)) | \
    (block_data_update? (0x1 << 3) : 0) | \
    (x_enable? 0x01 : 0) | \
    (y_enable? (0x01 << 1) : 0) | \
    (z_enable? (0x01 << 2) : 0) \
)

#define LSM303D_ctrl2(scale) ( \
    (scale == 2)? (0x0 << 3): \
    (scale == 4)? (0x1 << 3): \
    (scale == 6)? (0x2 << 3): \
    (scale == 8)? (0x3 << 3): \
    (0x4 << 3) \
)  // Defaulting to biggest scale
    
#define LSM303D_ctrl5(temp_enable, magnetic_resolution, magnetic_freq) ( \
    LSM303D_modr(magnetic_freq) | \
    (temp_enable? (0x01 << 7) : 0) | \
    (magnetic_resolution? (0x3 << 5) : 0) \
)

#define LSM303D_ctrl6(magnetic_scale) ( \
    (magnetic_scale == 2)? (0x0 << 5): \
    (magnetic_scale == 4)? (0x1 << 5): \
    (magnetic_scale == 8)? (0x2 << 5): \
    (magnetic_scale == 12)? (0x3 << 5): \
    (0x0 << 5) \
)
    
#define LSM303D_ctrl7(magnetic_mode) ( \
    (magnetic_mode) \
)
    
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

// End of setup macros

#define LSM303D_range 4
#define LSM303D_frequency 200
#define LSM303D_FIFO_READ 0x80


int LSM303D_init() {
    I2C_Start();
    
    uint8_t wai;
    int status = 0;
    status = I2C_Read(LSM303D, LSM303D_WHO_AM_I, &wai);
    
    if (wai != LSM303D_WHO_AM_I_REF) {
        return -1;
    }
    // Beginning startup sequence
    
    // FIFO enabled, FIFO limit off
    status = I2C_Write(LSM303D, LSM303D_CTRL0, LSM303D_ctrl0(0, 1, 0)); 
    // Setting accelerometer frequency, only x axis enabled 
    status = I2C_Write(LSM303D, LSM303D_CTRL1, LSM303D_ctrl1(LSM303D_frequency, 0, 1, 0, 0));
    // Setting accelerometer range
    status = I2C_Write(LSM303D, LSM303D_CTRL2, LSM303D_ctrl2(LSM303D_range));
    // Setting accelerometer range
    status = I2C_Write(LSM303D, LSM303D_CTRL2, LSM303D_ctrl2(LSM303D_range));
    // Enabling FIFO in stream mode, setting FIFO threshold
    status = I2C_Write(LSM303D, LSM303D_FIFO_CTRL, LSM303D_fifo_ctrl(2, 30));
    
    // Startup sequence complete    
    return status;
}



void LSM303D_Read_Acc(accelerometer_data* data){
    
    uint8_t xyz[6];
    
    I2C_Read_Multiple(LSM303D, LSM303D_OUT_X_L_A | LSM303D_FIFO_READ, xyz, 6);
    
    data->x = (int16)(xyz[1] << 8 | xyz[0]);
    data->y = (int16)(xyz[3] << 8 | xyz[2]);
    data->z = (int16)(xyz[5] << 8 | xyz[4]);
}

/* [] END OF FILE */

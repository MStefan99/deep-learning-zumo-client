/**
 * @file    I2C_made.c
 * @brief   Basic methods for I2C communication. For more details, please refer to I2C_made.h file. 
 * @details 
*/
#include "I2C_Common.h"


/**
* @brief    Function for generic I2C byte write
* @details  Single byte write 
* @param    uint8 device : Slave Device address
* @param    uint8 regAdd : register address to be written to
* @param    uint8 data : value to be written
*/
int I2C_Write(uint8_t device_addr, uint8_t reg_addr, uint8_t data) {
    int status = 0;
    
    I2C_MasterSendStart(device_addr, 0);
    if (!status) {
        status = I2C_MasterWriteByte(reg_addr);
    }
    if (!status) {
        status = I2C_MasterWriteByte(data);
    }
    I2C_MasterSendStop();
    
    return status;
}

/**
* @brief    Function for generic I2C single byte read
* @details  Single byte read 
* @param    uint8 device : Slave Device address
* @param    uint8 regAdd : register address to be read from
*/

int I2C_Read(uint8_t device_addr, uint8_t reg_addr, uint8_t* data) {
    
    int status = I2C_Read_Multiple(device_addr, reg_addr, data, 1);
    return status;
}

/**
* @brief    Function for generic I2C multi byte read
* @details  Multiple byte read 
* @param    uint8 device : Slave Device address
* @param    uint8 startAdd : The address of the first register to be read. Subsequent reads will auto increment this 
* @param    uint8 data : Pointer for value/values to be read
* @param    uint8 num_bytes_to_be_read : number of bytes to be read 
*/

int I2C_Read_Multiple(uint8_t device_addr, uint8_t reg_addr, uint8_t* data, uint8_t count) {
    int status = 0;
    
    status = I2C_MasterSendStart(device_addr, 0);
    if (!status) {
        status = I2C_MasterWriteByte(reg_addr);
    }
    if (!status) {
        status = I2C_MasterSendRestart(device_addr, 1);
    }
    if (!status) {
        for (int i = 0; i < count; i++) {
            data[i] = I2C_MasterReadByte(i == count - 1 ? I2C_2_NAK_DATA:I2C_2_ACK_DATA);
        }
    }
    I2C_MasterSendStop();
    
    return status;
}


int I2C_2_Read_Multiple(uint8_t device_addr, uint8_t reg_addr, uint8_t* data, uint8_t count) {
    int status = 0;
    if (I2C_DEBUG) {
        printf("Reading data from device %#04x, register %#04x...\n", device_addr, reg_addr);
    }
    
    status = I2C_2_MasterSendStart(device_addr, 0);
    if (!status) {
        status = I2C_2_MasterWriteByte(reg_addr);
    }
    if (!status) {
        status = I2C_2_MasterSendRestart(device_addr, 1);
    }
    if (!status) {
        for (int i = 0; i < count; i++) {
            data[i] = I2C_2_MasterReadByte(i == count - 1 ? I2C_2_NAK_DATA:I2C_2_ACK_DATA);
        }
    }
    I2C_2_MasterSendStop();
    
    if (I2C_DEBUG) {
        if (status) {
            printf("Reading failed. Status: %i\n", status);
        } else {
            printf("Read %i bytes from %#04x:\n", I2C_2_MasterGetReadBufSize(), reg_addr);
            for (int i = 0; i < I2C_2_MasterGetReadBufSize(); i++) {
                printf("    %#04x\n", data[i]);
            }
        }
    }
    return status;
}


int I2C_2_Write_Multiple(uint8_t device_addr, uint8_t reg_addr, uint8_t* data, uint8_t count) {
    int status = 0;
    if (I2C_DEBUG) {
        printf("Writing data to device %#04x, register %#04x...\n", device_addr, reg_addr);
    }
    
    I2C_2_MasterSendStart(device_addr, 0);
    if (!status) {
        status = I2C_2_MasterWriteByte(reg_addr);
    }
    if (!status) {
        for (int i = 0; i < count; i++) {
            status = I2C_2_MasterWriteByte(data[i]);
        }
    }
    I2C_2_MasterSendStop();
    
    if (I2C_DEBUG) {
        if (status) {
            printf("Writing failed. Status: %i\n", status);
        } else {
            printf("Wrote %i bytes to %#04x:\n", I2C_2_MasterGetWriteBufSize(), reg_addr);
            for (int i = 0; i < I2C_2_MasterGetWriteBufSize(); i++) {
                printf("    %#04x\n", data[i]);
            }
        }
    }
    return status;
}

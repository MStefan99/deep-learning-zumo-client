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


VL53L0X_Error Status = VL53L0X_ERROR_NONE;
VL53L0X_Dev_t MyDevice;
VL53L0X_Version_t Version;
VL53L0X_DeviceInfo_t DeviceInfo;


void print_vl53l0x_error(VL53L0X_Error Status){
    char buf[VL53L0X_MAX_STRING_LENGTH];
    VL53L0X_GetPalErrorString(Status, buf);
    printf("API Status: %i : %s\n", Status, buf);
}


void vl53l0x_check() {
    I2C_2_Start();
    
    printf("Reference registers:\n");
    uint8_t ref[4] = {0};
    
    VL53L0X_read_multi(0x52, 0xC0, (uint8_t *)&ref, 3);
    for (int i = 0; i < 3; ++i) {
        printf("  %#04x: %#04x\n", 0xC0 + i, ref[i]);
    }
    printf("\n");
    
    VL53L0X_read_multi(0x52, 0x51, (uint8_t *)&ref, 4);
    for (int i = 0; i < 4; i += 2) {
        printf("  %#04x: %#06x\n", 0x51 + i, (ref[i] << 4) + ref[i + 1]);
    }
    printf("\n");
}


void vl53l0x_init() {
    I2C_2_Start();
    
    int status_int = 0;
    
    MyDevice.I2cDevAddr = 0x52;
    MyDevice.comms_type = 1;
    MyDevice.comms_speed_khz = 100;
    
    if(Status == VL53L0X_ERROR_NONE) {
        status_int = VL53L0X_GetVersion(&Version);
        if(status_int != 0) {
            Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        }
    }
    
    
    if(Status == VL53L0X_ERROR_NONE) {
        printf ("Calling VL53L0X_DataInit\n");
        Status = VL53L0X_DataInit(&MyDevice); // Data initialization
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        printf ("Calling VL53L0X_GetDeviceInfo\n");
        Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        printf("VL53L0X_GetDeviceInfo:\n");
        printf("Device Name : %s\n", DeviceInfo.Name);
        printf("Device Type : %s\n", DeviceInfo.Type);
        printf("Device ID : %s\n", DeviceInfo.ProductId);
        printf("ProductRevisionMajor : %d\n", DeviceInfo.ProductRevisionMajor);
        printf("ProductRevisionMinor : %d\n", DeviceInfo.ProductRevisionMinor);

        if((DeviceInfo.ProductRevisionMinor != 1) && (DeviceInfo.ProductRevisionMinor != 1)) {
        	printf("Error expected cut 1.1 but found cut %d.%d\n",
        			DeviceInfo.ProductRevisionMajor, DeviceInfo.ProductRevisionMinor);
        	Status = VL53L0X_ERROR_NOT_SUPPORTED;
        }
    }
    print_vl53l0x_error(Status);
}


int vl53l0x_measure() {
    return 0;
}

/* [] END OF FILE */

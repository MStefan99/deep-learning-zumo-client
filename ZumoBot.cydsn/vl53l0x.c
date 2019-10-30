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
    printf("VL53L0X API Error: %i : %s\n", Status, buf);
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


int vl53l0x_init() {
    I2C_2_Start();
    
    int status_int = 0;
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;
    uint32_t refSpadCount;
    uint8_t isApertureSpads;
    uint8_t VhvSettings;
    uint8_t PhaseCal;
    
    // I2C init
    MyDevice.I2cDevAddr = 0x52;
    MyDevice.comms_type = 1;
    MyDevice.comms_speed_khz = 400;
    
    if(Status == VL53L0X_ERROR_NONE) {
        status_int = VL53L0X_GetVersion(&Version);
        if(status_int != 0) {
            Status = VL53L0X_ERROR_CONTROL_INTERFACE;
        }
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_DataInit(&MyDevice);
    } else {
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_GetDeviceInfo(&MyDevice, &DeviceInfo);
    } else {
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE && (DeviceInfo.ProductRevisionMinor != 1) && (DeviceInfo.ProductRevisionMinor != 1)) {
    	Status = VL53L0X_ERROR_NOT_SUPPORTED;
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_StaticInit(&MyDevice);
    } else {
        print_vl53l0x_error(Status);
    }
    
    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_PerformRefCalibration(&MyDevice, &VhvSettings, &PhaseCal);
    } else {
        print_vl53l0x_error(Status);
    }

    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_PerformRefSpadManagement(&MyDevice, &refSpadCount, &isApertureSpads);
    } else {
        print_vl53l0x_error(Status);
    }

    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_SetDeviceMode(&MyDevice, VL53L0X_DEVICEMODE_SINGLE_RANGING);
    } else {
        print_vl53l0x_error(Status);
    }
    
    // Final check
    if(Status == VL53L0X_ERROR_NONE) {
		Status = VL53L0X_StartMeasurement(&MyDevice);
    } else {
        print_vl53l0x_error(Status);
    }
    
    if (Status != VL53L0X_ERROR_NONE) {
        return 1;
    } else {
        return 0;
    }
}


int vl53l0x_measure() {
    uint16_t dist = -1;
    VL53L0X_RangingMeasurementData_t RangingMeasurementData;
    
    if(Status == VL53L0X_ERROR_NONE) {
        Status = VL53L0X_PerformSingleRangingMeasurement(&MyDevice, &RangingMeasurementData);
        dist = RangingMeasurementData.RangeMilliMeter;
    }
    
    if (Status == VL53L0X_ERROR_NONE && dist < 8100) {
        return dist;
    } else {
        return -1;
    }
}

/* [] END OF FILE */

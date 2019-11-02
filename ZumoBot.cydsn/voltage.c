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
#include "voltage.h"


bool low_voltage_detected = false;


float battery_voltage() {
    float result = 0;
    const float battery_voltage_convertion_coeffitient = 1.5;
    const float level_convert_coefficient = 5.0/4095.0;
    
    ADC_Battery_IsEndConversion(ADC_Battery_WAIT_FOR_RESULT);
    int16_t adc_value = ADC_Battery_GetResult16();
    
    result = adc_value * battery_voltage_convertion_coeffitient * level_convert_coefficient;

    return result;
}

int voltage_test() {
    float voltage = battery_voltage();
        
    if (voltage > 3.8) {
        return 0;
    } else {
        return 1;
    } 
}


void voltage_task() {
    while(1){
        if (voltage_test() && !low_voltage_detected) {
            mqtt_print("Info/Zumo/WARNING", "Low voltage: %3.2fV!", battery_voltage());
            low_voltage_detected = true;
            change_state(ERR_STATE);
        } else if (!voltage_test() && low_voltage_detected) {
            mqtt_print("Info/Zumo/Status", "Voltage normal");
            low_voltage_detected = false;
            change_state(PREV_STATE);
        }
        vTaskDelay(100);
    }
}


/* [] END OF FILE */

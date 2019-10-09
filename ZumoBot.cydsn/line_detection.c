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

#include "line_detection.h"


ref_offset offset = {0, 0, 0};


void ref_read_normalized(sensors *ref, ref_offset off) {
    reflectance_read(ref);
    reflectance_normalize(ref, off);
}


int intersection_detected() {
    struct sensors_ ref;
    const uint16_t threshold = 20000;
    
    reflectance_read(&ref);
    if(ref.l3 + ref.l2 + ref.l1 + ref.r1 + ref.r2 + ref.r3 > threshold * 4) {
        return 1;
    }
    return 0;
}


int line_centered() {
    struct sensors_ ref;
    const uint16_t threshold = 2000;
    
    reflectance_read(&ref);
    return abs(ref.l1 - ref.r1) < threshold;
}


ref_offset ref_get_offset(sensors ref_readings) {
    ref_offset diff;
    
    diff.inner = ref_readings.r1 - ref_readings.l1; 
    diff.mid = ref_readings.r2 - ref_readings.l2; 
    diff.outer = ref_readings.r3 - ref_readings.l3; 
    return diff;
}


void calibrate(ref_offset *off) {
    sensors values;
    reflectance_read(&values);
    *off = ref_get_offset(values);
}


void reflectance_normalize(sensors *ref, ref_offset off) {
    ref->r1 -= off.inner;
    ref->r2 -= off.mid;
    ref->r3 -= off.outer;
}


int get_line_pos() {
    sensors ref;
    reflectance_read(&ref);
    
    int setpoint_value_inner = 21500;
    int setpoint_value_outer = 5000;
    int threshold = 15000;
    
    // white - 4200
    // black - 23600
    
    int delta_r1 = ref.r1 - setpoint_value_inner;
    int delta_l1 = setpoint_value_inner - ref.l1;
    int delta_r2 = ref.r2 - setpoint_value_outer;
    int delta_l2 = setpoint_value_outer - ref.l2;
    int delta_r3 = ref.r3 - setpoint_value_outer;
    int delta_l3 = setpoint_value_outer - ref.l3;
    
    if (ref.r1 > threshold && ref.l1 > threshold) {
        return 0;
    }
    
    return (delta_r1 + delta_l1 + delta_r2 + delta_l2 + delta_r3 + delta_l3) / 180;
}


/* [] END OF FILE */

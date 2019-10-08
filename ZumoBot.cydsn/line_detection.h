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

#ifndef LINE_DETECTION
#define LINE_DETECTION

    #include <stdbool.h>
    #include <Reflectance.h>
    
    typedef struct reflectance_offset_ {
        int16_t sensor1;
        int16_t sensor2;
        int16_t sensor3;    
    } reflectance_offset;
    
    // Sets the calibration between right and left sensors
    reflectance_offset reflectance_calibrate(sensors *ref_readings); 
    
    // Edits the reflectance readings according to previous calibration
    void reflectance_normalize(sensors *ref_readings, reflectance_offset *ref_offset); 
    
    // Returns offset from the line
    int get_offset(sensors *ref_readings);
    
    // Returns 0 if on line, 1 if line is on the right and -1 if on the left
    int is_following_line();
    
    // Returns change in offset from the line
    int get_offset_change(sensors *ref_readings);
    
    // Detects cross 
    bool cross_detected();
    
#endif

/* [] END OF FILE */

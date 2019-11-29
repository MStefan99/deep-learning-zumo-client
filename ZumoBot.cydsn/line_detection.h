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
    
    #include <Reflectance.h>
    #include <stdlib.h>
    
    typedef struct {
        int16_t inner;
        int16_t mid;
        int16_t outer;
    } ref_offset;
    
    extern ref_offset offset;
    
    
    void calibrate_ref();
    void ref_read_normalized(sensors *ref);
    
    int get_line_pos();
    int intersection_detected();
    int line_centered();
    
#endif

/* [] END OF FILE */

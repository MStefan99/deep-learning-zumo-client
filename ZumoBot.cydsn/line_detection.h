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
    
    
    void ref_read_normalized(sensors *ref, ref_offset off);
    ref_offset ref_get_offset(sensors ref); 
    void calibrate();
    void reflectance_normalize(sensors *ref, ref_offset off); 
    int get_line_pos();
    int intersection_detected();
    int line_centered();
    
#endif

/* [] END OF FILE */

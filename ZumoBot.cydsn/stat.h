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

#ifndef STAT
    #define STAT
    
    #include <math.h>

    
    double expected_value(const double *realizations, int n);
    double variance(double *realizations, double exp_value, int n);
    double standard_deviation(double variance);
    
#endif

/* [] END OF FILE */

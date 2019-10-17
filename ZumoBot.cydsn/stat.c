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

#include "stat.h"


double expected_value(const double *realizations, int n) {
    double e;
    double sum = 0;

    for (int i = 0; i < n; ++i) {
        sum += realizations[i];
    }
    e = sum/n;
    return e;
}


double variance(double *realizations, double exp_value, int n) {
    double v;
    double d[n];

    for (int i = 0; i < n; ++i) {
        d[i] = pow(realizations[i] - exp_value, 2);
    }
    v = expected_value(d, n);
    return v;
}


double std_deviation(double variance) {
    double d;
    d = sqrt(variance);
    return d;
}


/* [] END OF FILE */

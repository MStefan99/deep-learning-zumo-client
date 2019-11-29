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

#ifndef MAIN
    #define MAIN
    
    #include <project.h>
    #include "FreeRTOS.h"
    #include "task.h"
    #include "Nunchuk.h"
    #include "vl53l0x.h"
    #include "Reflectance.h"
    #include "Beep.h"
    #include <time.h>
    #include <sys/time.h>
    #include "serial1.h"
    #include <unistd.h>
    #include "line_detection.h"
    #include "log.h"
    #include "movement.h"
    #include "smqtt.h"
    #include <stdbool.h>
    #include "stat.h"
    #include "scan.h"
    #include "states.h"
    
    
    CY_ISR_PROTO(button_isr);
    CY_ISR_PROTO(led_isr);
    
#endif

/* [] END OF FILE */

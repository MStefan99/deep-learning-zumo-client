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


#ifndef STATES
    #define STATES
    
    #include "movement.h"
    #include "smqtt.h"
    
    
    typedef enum {
        ERR_STATE = -1,
        PREV_STATE,
        BOOT_IDLE_STATE,
        CLB_IDLE_STATE,
        WAIT_STATE,
        PRESCAN_STATE,
        NAV_STATE,
        CMP_NAV_STATE,
        FIN_IDLE_STATE,
        LOCK_STATE,
        LOW_VOLTAGE_STATE
    } state_id_t;
    
    
    typedef struct {
        state_id_t id;
        char name[32];
        int movement_enabled;
        int led_timings[10];
    } state;
    
    
    extern const state states[10];
    extern volatile state_id_t current_state;
    extern volatile int led_state;
    
    void change_state(state_id_t state);
    int find_state(state_id_t state);
    
#endif

/* [] END OF FILE */

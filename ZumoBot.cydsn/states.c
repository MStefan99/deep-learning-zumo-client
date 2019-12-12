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

#include "states.h"


const state states[10] = {
        {ERR_STATE, "Error", 0, {300, 50, 100, 50, 0}},  // State 8 - error
        {BOOT_IDLE_STATE, "Boot idle", 0, {50, 1450, 0}}, // State 0 - idle (booted)
        {CLB_IDLE_STATE, "Clb idle", 0, {50, 150, 50, 1250, 0}},  // State 1 - idle (calibrated)
        {WAIT_STATE, "Wait", 0, {900, 100, 100, 100, 800, 0}},  // State 2 - waiting for server
        {PRESCAN_STATE, "Pre-scan", 1, {300, 1700, 0}},  // State 3 - pre-start scanning
        {NAV_STATE, "Nav", 1, {300, 200, 300, 1200, 0}},  // State 4 - server-guided navigation
        {CMP_NAV_STATE, "Cmp nav", 1, {300, 200, 300, 200, 300, 700}},  // State 5 - track completion
        {FIN_IDLE_STATE, "Finish idle", 0, {50, 150, 50, 150, 50, 1050, 0}},  // State 6 - idle (finished)
        {LOCK_STATE, "Locked", 0, {125, 125, 0}},  // State 7 - motor locked
        {LOW_VOLTAGE_STATE, "Low voltage", 0, {450, 50, 0}}  // State 9 - low voltage
};
volatile state_id_t current_state = ERR_STATE;
volatile state_id_t prev_state = ERR_STATE;
volatile int led_state = 0;


void change_state(state_id_t state) {
    if (state != PREV_STATE) {
        if (state != current_state) {
            prev_state = find_state(current_state);
            current_state = find_state(state);
        }
    } else {
        int t = find_state(prev_state);
        prev_state = find_state(current_state);
        current_state = t;
    }
    set_motor_state(states[find_state(current_state)].movement_enabled);
    
    mqtt_print("Info/Zumo/State", "\"%s\"", states[find_state(current_state)].name);
    led_state = 0;
}


int find_state(state_id_t state) {
    for(int i = 0; i < (int)(sizeof(states) / sizeof(states[0])); ++i) {
        if (state == states[i].id) {
            return i;
        }
    }
    return -1;
}



/* [] END OF FILE */

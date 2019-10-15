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

#ifndef SMQTT
    #define SMQTT
    
    #include "MQTTFreeRTOS.h"
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"

    #include "MQTTClient.h"
    #include "zumo_config.h"
    #include <stdio.h>
    
    #define MAX_LEN 32
    
    
    typedef struct {
        char topic[MAX_LEN];
        char message[MAX_LEN];
    } mqtt_message;
    
    
    void SMQTTReceive(MessageData *msg);
    void SMQTTQueueInit();
    void SMQTTTask();
    void SMQTTInit();
    void mqtt_send(mqtt_message msg);
    void mqtt_receive(mqtt_message *msg);
    
    
    
    #endif

/* [] END OF FILE */

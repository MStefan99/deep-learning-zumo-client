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
    #include "stdarg.h"
    #include <stdio.h>
    
    #define MESSAGE_SIZE 32
    
    
    typedef struct {
        char topic[MESSAGE_SIZE];
        char message[MESSAGE_SIZE];
    } mqtt_message;
    
    
    int mqtt_print(char *topic, char *format, ...);
    int mqtt_sub(char *topicFilter);
    int mqtt_receive(mqtt_message *msg);
    int mqtt_unsub(char *topicFilter);
    
    
    void SMQTTReceive(MessageData *msg);
    void SMQTTQueueInit();
    void SMQTTTask();
    void mqtt_send(mqtt_message msg);
    
    #endif

/* [] END OF FILE */

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
    
    #include <stdio.h>
    #include <stdarg.h>
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"
    #include "MQTTClient.h"
    #include "MQTTFreeRTOS.h"
    #include "zumo_config.h"
    
    #define MESSAGE_SIZE 32
    
    typedef struct {
        char topic[MESSAGE_SIZE];
        char message[MESSAGE_SIZE];
    } mqtt_message;
    
    
    int mqtt_print(char *topic, char *format, ...);
    int mqtt_sub(char *topicFilter);
    int mqtt_receive(mqtt_message *msg);
    int mqtt_peek(mqtt_message *msg);
    int mqtt_check();
    int mqtt_unsub(char *topicFilter);
    void mqtt_send(mqtt_message msg);
    
    
    // DON'T USE THE FOLLOWING FUNCTIONS!
    void SMQTTReceive(MessageData *msg);
    void SMQTTQueueInit();
    void SMQTTTask();
    
#endif

/* [] END OF FILE */

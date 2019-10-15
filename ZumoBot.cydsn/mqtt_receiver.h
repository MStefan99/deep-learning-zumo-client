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

#ifndef MQTT_RECEIVER_
    #define MQTT_RECEIVER_
    #include "MQTTFreeRTOS.h"
    #include "FreeRTOS.h"
    #include "task.h"
    #include "queue.h"

    #include "MQTTClient.h"
    #include "zumo_config.h"
    
    typedef struct {
        const char *topic;
        char *message;
    } mqtt_message;
    
    
    void handler(MessageData *msg);
    void MQTTReceiveTask();
    
    #endif

/* [] END OF FILE */

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

#include "smqtt.h"


#define MAX_MESSAGE_COUNT 16


typedef struct {
    int sub;
    char topic[MESSAGE_SIZE];
} sub_entry;


QueueHandle_t in_q;
QueueHandle_t out_q;
QueueHandle_t sub_q;

MQTTClient client;
Network network;

mqtt_message buf_out;
mqtt_message buf_in;


void SMQTTReceive(MessageData *msg) {
    strncpy(buf_in.topic, msg->topicName->lenstring.data, MESSAGE_SIZE);
    strncpy(buf_in.message, msg->message->payload, MESSAGE_SIZE);
    
    buf_in.topic[MESSAGE_SIZE - 1] = 0;
    buf_in.message[MESSAGE_SIZE - 1] = 0; 
    
    xQueueSendToBack(in_q, &buf_in, 0);
}


void SMQTTQueueInit() {
    in_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
    out_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
    sub_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(sub_entry));
}


void SMQTTTask() {
    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    unsigned char sendbuf[128], readbuf[128];
    int rc = 0;
    connectData.MQTTVersion = 3;
	connectData.clientID.cstring = MQTT_CLIENT_ID;
    char* address = MQTT_BROKER;
    
    NetworkInit(&network, NETWORK_SSID, NETWORK_PASSWORD);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
    
	if ((rc = NetworkConnect(&network, address, 1883)) != 0) {
		printf("Return code from network connect is %d\n", rc);
    }
    #if defined(MQTT_TASK)
    	if ((rc = MQTTStartTask(&client)) != pdPASS) {
    		printf("Return code from start tasks is %d\n", rc);
        }
    #else
        #error "MQTT_TASK not defined"
    #endif
    
	if ((rc = MQTTConnect(&client, &connectData)) != 0) {
		printf("Return code from MQTT connect is %d\n", rc);
    }
    
    sub_entry s;
    MQTTMessage buf_m;
    buf_m.qos = QOS0;
    buf_m.retained = 0;
    
    while (1) {
        while (xQueueReceive(sub_q, (void *)&s, 0) == pdTRUE) {
            if (s.sub) {
                MQTTSubscribe(&client, s.topic, 2, SMQTTReceive);
                printf("SMQTT subscribed to \"%s\"\n", s.topic);
            } else {
                MQTTUnsubscribe(&client, s.topic);
                printf("SMQTT unsubscribed from \"%s\"\n", s.topic);
            }
        }
        
        while (xQueueReceive(out_q, (void *)&buf_out, 0) == pdTRUE) {
            buf_m.payload = buf_out.message;
            buf_m.payloadlen = strlen(buf_out.message);
            
            MQTTPublish(&client, buf_out.topic, &buf_m);
        }
        vTaskDelay(1000);
    }
}


int mqtt_print(char *topic, char *format, ...) {
    va_list va;
    char buf[MESSAGE_SIZE];
    
    va_start(va, format);
    int n = vsnprintf(buf, MESSAGE_SIZE, format, va);
    va_end(va);
    mqtt_message msg;
    strncpy(msg.topic, topic, MESSAGE_SIZE);
    strncpy(msg.message, buf, MESSAGE_SIZE);
    
    msg.topic[MESSAGE_SIZE - 1] = 0;
    msg.message[MESSAGE_SIZE - 1] = 0;
    
    mqtt_send(msg);
    return n;
}


int mqtt_sub(char *topicFilter) {
    sub_entry s;
    s.sub = 1;
    strncpy(s.topic, topicFilter, MESSAGE_SIZE);
    s.topic[MESSAGE_SIZE - 1] = 0;
    
    return xQueueSendToBack(sub_q, (void *)&s, 0);
}


int mqtt_unsub(char *topicFilter) {
    sub_entry s;
    s.sub = 0;
    strncpy(s.topic, topicFilter, MESSAGE_SIZE);
    s.topic[MESSAGE_SIZE - 1] = 0;
    
    return xQueueSendToBack(sub_q, (void *)&s, 0);
}


void mqtt_send(mqtt_message msg) {
    xQueueSendToBack(out_q, (void *)&msg, 0);
}


int mqtt_receive(mqtt_message *msg) {
    if (xQueueReceive(in_q, msg, 10) != pdTRUE) {
        return 0;
    }
    return 1;
}


int mqtt_peek(mqtt_message *msg) {
    if (xQueuePeek(in_q, msg, 10) != pdTRUE) {
        return 0;
    }
    return 1;
}


int mqtt_check() {
    return uxQueueMessagesWaiting(in_q);
}


/* [] END OF FILE */

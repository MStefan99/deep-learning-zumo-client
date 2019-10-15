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
static QueueHandle_t in_q;
static QueueHandle_t out_q;
MQTTClient client;
Network network;

mqtt_message buf_out;
mqtt_message buf_in;


void SMQTTReceive(MessageData *msg) {
    uint8_t i;
    for (i = 0; i < msg->topicName->lenstring.len; ++i) {
        buf_in.topic[i] = msg->topicName->lenstring.data[i];
    }
    buf_in.topic[i] = 0;
    
    for (i = 0; i < msg->message->payloadlen; ++i) {
        buf_in.message[i] = ((char *)msg->message->payload)[i];
    }
    buf_in.message[i] = 0;
    
    xQueueSendToBack(in_q, &buf_in, 0);
}


void SMQTTQueueInit() {
    in_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
    out_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
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
    
	if ((rc = NetworkConnect(&network, address, 1883)) != 0)
		printf("Return code from network connect is %d\n", rc);
    #if defined(MQTT_TASK)
    	if ((rc = MQTTStartTask(&client)) != pdPASS)
    		printf("Return code from start tasks is %d\n", rc);
    #else
        #error "MQTT_TASK not defined"
    #endif
    
	if ((rc = MQTTConnect(&client, &connectData)) != 0)
		printf("Return code from MQTT connect is %d\n", rc);
	else
		printf("MQTT Connected\n");
    
    while (1) {
        MQTTMessage buf_m;
        buf_m.qos = QOS0;
        buf_m.retained = 0;
        
        if (xQueueReceive(out_q, (void *)&buf_out, 0) == pdTRUE) {
            buf_m.payload = buf_out.message;
            buf_m.payloadlen = strlen(buf_out.message);
            
            MQTTPublish(&client, buf_out.topic, &buf_m);
        }
    }
}


int mqtt_print(char *topic, char *format, ...) {
    va_list va;
    char buf[MESSAGE_SIZE];
    
    va_start(va, format);
    int n = vsnprintf(buf, MESSAGE_SIZE, format, va);
    va_end(va);
    mqtt_message msg;
    strcpy(msg.topic, topic);
    strcpy(msg.message, buf);
    
    mqtt_send(msg);
    return n;
}


int mqtt_sub(char *topicFilter) {
    if (MQTTIsConnected(&client)) {
        return MQTTSubscribe(&client, topicFilter, 2, SMQTTReceive);
    } else {
        return 0;
    }
}


int mqtt_unsub(char *topicFilter) {
    if (MQTTIsConnected(&client)) {
        return MQTTUnsubscribe(&client, topicFilter);
    } else {
        return 0;
    }
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


/* [] END OF FILE */

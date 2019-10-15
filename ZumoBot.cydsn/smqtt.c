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
#define BUFFER_SIZE 80
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
    
    printf("Received message on topic \"%s\": \"%s\"\n", buf_in.topic, buf_in.message);
    xQueueSendToBack(in_q, &buf_in, 0);
}


void SMQTTQueueInit() {
    printf("Setting up MQTT message queue.\n");
    in_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
    out_q = xQueueCreate(MAX_MESSAGE_COUNT, sizeof(mqtt_message));
}


void SMQTTTask() {
    printf("Starting MQTT\n");
    MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
    unsigned char sendbuf[128], readbuf[128];
    int rc = 0;
    connectData.MQTTVersion = 3;
	connectData.clientID.cstring = MQTT_CLIENT_ID;
    char* address = MQTT_BROKER;
    
    printf("Connectig to network\n");
    NetworkInit(&network, NETWORK_SSID, NETWORK_PASSWORD);
    printf("Setting up MQTT client\n");
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
    
    if ((rc = MQTTSubscribe(&client, "#", 2, SMQTTReceive)) != 0)
		printf("Return code from MQTT subscribe is %d\n", rc);
    else {
        printf("MQTT Subscribed\n");
    } 
    printf("SMQTT Task started\n");
    
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


void mqtt_send(mqtt_message msg) {
    printf("Adding message to queue\n");
    xQueueSendToBack(out_q, (void *)&msg, 0);
}


void mqtt_receive(mqtt_message *msg) {
    printf("Receiving message from queue\n");
    if ( xQueueReceive(in_q, &msg, 0) != pdTRUE) {
        msg = NULL;
    }
}


/* [] END OF FILE */

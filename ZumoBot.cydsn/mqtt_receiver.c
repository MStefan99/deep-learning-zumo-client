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

#include "mqtt_receiver.h"


void handler(MessageData *msg) {
    printf("Message received: \"%s\"\n", msg->message->payload);
}

void MQTTReceiveTask() {
    MQTTClient client;
    Network network;
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
    
    if ((rc = MQTTSubscribe(&client, "#", 2, handler)) != 0)
		printf("Return code from MQTT subscribe is %d\n", rc);
    else {
        printf("MQTT Subscribed\n");
    }
    
    while (1) {
    }
}


/* [] END OF FILE */

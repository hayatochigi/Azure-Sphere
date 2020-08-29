#include <applibs/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Eclipse Paho C/include/MQTTClient.h"
// ThingSpeakのMQTTブローカーへトピックをパブリッシュします。
// MQTT の基礎https://jp.mathworks.com/help/thingspeak/mqtt-basics.html
// "ThingSpeak™ では、MQTT ブローカーが mqtt.thingspeak.com の URL にあり、ポートは 1883 です。"

#define ADDRESS     "tcp://localhost:1883"
#define CLIENTID    "ExampleClientPub"
#define TOPIC       "MQTT Examples"
#define PAYLOAD     "Hello World!"
#define QOS         1
#define TIMEOUT     10000L

int main(void)
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    MQTTClient_deliveryToken token;
    int rc;

    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    

    return rc;
}

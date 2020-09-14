// MQTT V3.1 プロトコル仕様
// http://public.dhe.ibm.com/software/dw/jp/websphere/wmq/mqtt31_spec/mqtt-v3r1_ja.pdf

// MQTT クライアント・アプリケーションでのコールバックと同期
// https://www.ibm.com/support/knowledgecenter/ja/SSFKSJ_9.1.0/com.ibm.mq.dev.doc/q029020_.htm

// “Internet of Things” モノとモノが繋がるインターネットで広がる”Connected Life”
// http://ngm2m.jp/m2m/files/symp2014_suzuki_pm.pdf


#include <stdio.h>
#include <string.h>
#include <applibs/log.h>
#include <stdbool.h>
#include "wolfmqtt/wolfmqtt/mqtt_client.h"

#define MAX_BUFFER_SIZE         1024
#define DEFAULT_CMD_TIMEOUT_MS  1000
#define BROCKER_TIMEOUT 5000
#define CLIENT_ID "fnaothi0"

// From ThingSpesk Settings
#define HOST "mqtt.thingspeak.com"              // ThingSpeak Broker URL
#define PORT 1883                           // MQTT Default Port 1883 https://jp.mathworks.com/help/thingspeak/mqtt-basics.html
#define USERNAME "emboar"                   // From my Profile page
#define PASSWORD "YLW76Y8V68WPAMZS"         // Change this your MQTT API Key from Account > MyProfile.
#define WRITE_API_KEY "8HK9Q8SVUOA4AZMH"    // My Channels > User Channel > API Keys 
#define ChannelID "1117561"                 // My Channels > User Channel > Channel Settings

/// <summary>
/// MQTT Callback Function (At this moment, this is NOP)
/// </summary>
static int mqttclient_message_cb(MqttClient* client, MqttMessage* msg, byte msg_new, byte msg_done)
{
    return MQTT_CODE_SUCCESS;
}

void ErrorHandler(char func_name[], enum MqttPacketResponseCodes error_code);

int main(void){

    int result = 0;

    MqttClient myMqttClient;
    memset(&myMqttClient, 0, sizeof(myMqttClient));
    MqttNet myMqttNet;
    memset(&myMqttNet, 0, sizeof(myMqttNet));
    byte* tx_buf = NULL, * rx_buf = NULL;
 
    // Send & Receive Buffer
    tx_buf = malloc(MAX_BUFFER_SIZE);
    rx_buf = malloc(MAX_BUFFER_SIZE);

    // Define calback functions when "CONNECT", "READ", "WRITE", "DISCONNECT" method is called.
    myMqttNet.connect = mqttclient_message_cb;
    myMqttNet.read = mqttclient_message_cb;
    myMqttNet.write = mqttclient_message_cb;
    myMqttNet.disconnect = mqttclient_message_cb;

#pragma region Initialize
    // Doc Step3: Call MqttClient_Init
    // MqttClient parameter could be empty.
    result = MqttClient_Init(&myMqttClient, &myMqttNet, mqttclient_message_cb,
        tx_buf, MAX_BUFFER_SIZE, rx_buf, MAX_BUFFER_SIZE, DEFAULT_CMD_TIMEOUT_MS);
    ErrorHandler("MqttClient_Init", result);

    // Doc Step4: Call MqttClient_NetConnect to establish a communication between a MQTT Broker
    /* cannot use TLS unless ENABLE_MQTT_TLS is defined */
    /* If TLS is required, specify certificated document in callback function */
    result = MqttClient_NetConnect(&myMqttClient, HOST, PORT, BROCKER_TIMEOUT, false, NULL);
    ErrorHandler("MqttClient_NetConnect", result);

    // Doc Step5: Create Mqtt Structure and call MqttClient_Connect   
    MqttConnect ConnectInfo;
    memset(&ConnectInfo, 0, sizeof(ConnectInfo));
    ConnectInfo.stat = MQTT_MSG_BEGIN;
 
    // Keep alive ensures that the connection between the brokerand client is 
    // still openand that the brokerand the client are aware of being connected.
    ConnectInfo.keep_alive_sec = 30;
    // When the clean session flag is set to true, the client does not want a persistent session.
    // If the client disconnects for any reason, all informationand messages that are queued from a previous persistent session are lost.
    ConnectInfo.clean_session = true;
    ConnectInfo.client_id = CLIENT_ID;

    // Protocol is v3.1.1. Use "4". This is written in a comment.
    ConnectInfo.protocol_level = 4;

    // Last Will & Testament
    // I don't want, disable.
    ConnectInfo.enable_lwt = false;
    //ConnectInfo.lwt_msg;

    ConnectInfo.username = USERNAME;
    ConnectInfo.password = PASSWORD;

    ConnectInfo.ack.stat = MQTT_MSG_WAIT;
    ConnectInfo.ack.flags = 0;
    ConnectInfo.ack.return_code = 0;

    result = MqttClient_Connect(&myMqttClient, &ConnectInfo);
    ErrorHandler("MqttClient_Connect", result);

    result = MqttClient_Ping(&myMqttClient);
    ErrorHandler("MqttClient_Ping", result);
#pragma endregion
      
#pragma region Publish

    MqttPublish myPublish;
    memset(&myPublish, 0, sizeof(myPublish));
    // retain: Retain last sent message.
    myPublish.retain = true;
    // qos: Quality Of Service。No Confirmation is required.
    myPublish.qos = MQTT_QOS_0;
    myPublish.duplicate = true;

    // Create a topic string and publish data to ThingSpeak channel feed. 
    // (Example)
    // String topicString = "channels/" + String(channelID) + "/publish/" + String(writeAPIKey);
    // https://www.mathworks.com/help/thingspeak/use-arduino-client-to-publish-to-a-channel.html?requestedDomain=
    myPublish.topic_name = "channels/1126475/publish/fields/field1/B3VHA7EQ4L8HN9NS";

    myPublish.packet_id = 0;
    char message[] = "8";
    myPublish.buffer = message;
    myPublish.buffer_len = sizeof(message);
    
    result = MqttClient_Publish(&myMqttClient, &myPublish);
    ErrorHandler("MqttClient_Publish", result);

    result = MqttClient_WaitMessage(&myMqttClient, BROCKER_TIMEOUT);
    ErrorHandler("MqttClient_WaitMessage", result);
#pragma endregion   

#pragma region Close
    result = MqttClient_Disconnect(&myMqttClient);
    ErrorHandler("MqttClient_Disconnect", result);
    result = MqttClient_NetDisconnect(&myMqttClient);
    ErrorHandler("MqttClient_NetDisconnect", result);

    free(tx_buf);
    free(rx_buf);
#pragma endregion

    return 0;
}

/// <summary>
/// Receive return from MQTT functions. If it isn't "SUCCESS", show the error information.
/// </summary>
/// <param name="func_name">Function Name</param>
/// <param name="error_code">Status Code</param>
void ErrorHandler(char func_name[], enum MqttPacketResponseCodes error_code) {
    if (error_code != MQTT_CODE_SUCCESS) {
        printf("%s: %s (%d)\n", func_name, MqttClient_ReturnCodeToString(error_code), error_code);
        return error_code;
    }
    else {
        printf("%s is completed\n", func_name);
    }
}

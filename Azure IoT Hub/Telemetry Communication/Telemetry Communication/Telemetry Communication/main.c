#include <applibs/log.h>
#include "iothub_device_client_ll.h"
#include "iothub_security_factory.h"
#include "iothubtransportmqtt.h"

#include <stdbool.h>
#include <time.h>

// IoT Hubの「概要」ページから
#define hubHostName "xxx.azure-devices.net"
// IoT Hubの「共有アクセスポリシー」ページから
#define ConnectionStr "HostName=xxx="
// IoT Hubの「IoTデバイス」ページから
#define deviceId "xxx"

// *************** 関数宣言 *************** //
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);
bool AzureIoTHubSend(IOTHUB_DEVICE_CLIENT_LL_HANDLE*, char*);
bool AzureIoTHubRecvSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);
IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE, void*);
// **************************************** //


int main(void)
{
    struct timespec Wait = { .tv_sec = 0, .tv_nsec = 5000000000 };

    // IoT Hubのクライアントハンドルを作成し、memsetで初期化
    static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClient;
    memset(&iothubClient, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));
    

    if (AzureIoTHubSetup(&iothubClient) == false) {
        return -1;
    }   

    if (AzureIoTHubRecvSetup(&iothubClient) == false) {
        return -1;
    }

    while(true){
        
        // メッセージを送信。"false"の戻り値は関数の失敗を示す
        if (AzureIoTHubSend(&iothubClient, "Message D2C") == false) {
            break;
        }
        nanosleep(&Wait, NULL);
    }

    // 実際のアプリケーションではIoT Hubとの接続を解除する
    // IoTHubDeviceClient_LL_Destroy(iothubClient);

    return 0;
}


/// <summary>
/// デバイス接続文字列を用いてMQTTプロトコルでIoT Hubへの接続クライアントを作成
/// </summary>
/// <param name="client">IOTHUB_DEVICE_CLIENT_LL_HANDLEへのポインタ</param>
/// <returns>成功: true   失敗: false</returns>
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE* client) {

    // デバイス接続文字列でクライアントハンドルを作成
    // IOTHUB_DEVICE_CLIENT_LL_HANDLEの値として変数へ格納
    *client = IoTHubDeviceClient_LL_CreateFromConnectionString(ConnectionStr, MQTT_Protocol);
    if (*client == NULL) {
        Log_Debug("Error at IoTHubDeviceClient_LL_CreateFromConnectionString\n");
        return false;
    }   

    return true;
}


/// <summary>
/// IoT Hubへメッセージを送信する
/// </summary>
/// <param name="client">IOTHUB_DEVICE_CLIENT_LL_HANDLEへのポインタ</param>
/// <param name="message">送信メッセージ文字列</param>
/// <returns>成功: true   失敗: false</returns>
bool AzureIoTHubSend(IOTHUB_DEVICE_CLIENT_LL_HANDLE* client, char* message) {

    // エラーステータス保持用の変数
    int retError;

    // 送信メッセージを作成
    IOTHUB_MESSAGE_HANDLE myMessage = IoTHubMessage_CreateFromString(message);

    // クライアントハンドルに、送信するメッセージを追加
    // この時点で送信が行われるのではなく、実際にはDoWorkで送信が実行される
    retError = IoTHubDeviceClient_LL_SendEventAsync(*client, myMessage, NULL, NULL);
    if (retError != IOTHUB_CLIENT_OK) {
        Log_Debug("Error at IotHubDeviceClient_LL_SendEventAsync\n");
        return false;
    }

    // メッセージを追加したら、そのメッセージハンドル自体は不要のため破棄
    IoTHubMessage_Destroy(myMessage);

    // 実行する
    IoTHubDeviceClient_LL_DoWork(*client);
    Log_Debug("Send Message: %s\n", message);

    return true;
}


/// <summary>
/// Azure IoT Hubからのメッセージを受信して実行するコールバック関数を定義
/// </summary>
/// <param name="client">IOTHUB_DEVICE_CLIENT_LL_HANDLEへのポインタ</param>
/// <returns>成功: true   失敗: false</returns>
bool AzureIoTHubRecvSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE* client) {

    if (IoTHubDeviceClient_LL_SetMessageCallback(*client, receive_msg_callback, NULL) != IOTHUB_CLIENT_OK) {
        return false;
    }

    return true;
}


/// <summary>
/// Azure IoT Hubからメッセージを受信した際に呼び出されるコールバック関数
/// </summary>
/// <param name="message"></param>
/// <param name="user_context"></param>
/// <returns></returns>
IOTHUBMESSAGE_DISPOSITION_RESULT receive_msg_callback(IOTHUB_MESSAGE_HANDLE message, void* user_context) {

    unsigned char* mssgbuff;
    size_t buffsize;

    // メッセージ成型用の文字列
    char recvMssg[255];

    // メッセージハンドルからメッセージバイト配列を取得し、変数へ格納
    IoTHubMessage_GetByteArray(message, (const unsigned char**)&mssgbuff, &buffsize);
    
    // メッセージバイト配列はサイズが実サイズより大きいので、
    // recvMssgにstrncpyすることで調整
    strncpy(recvMssg, mssgbuff, buffsize);
    recvMssg[buffsize] = '\x0';
    Log_Debug("IoT Hub send message: %s\n", recvMssg);

    // IoT Hub上でKey = "Name" Value = "MyName"のプロパティを定義し、
    // 「プロパティを追加」からプロパティを追加する
    Log_Debug("Property: Name, Value = %s\n", IoTHubMessage_GetProperty(message, "Name"));

    return IOTHUBMESSAGE_ACCEPTED;
}

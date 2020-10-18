#include <applibs/log.h>
#include "iothub_device_client_ll.h"
#include "iothub_security_factory.h"
#include "iothubtransportmqtt.h"
#include "parson.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define hubHostName "xxx.azure-devices.net"
#define ConnectionStr ""
#define deviceId "AzMT3620"

// *************** 関数宣言 *************** //
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);

static void twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad,
    size_t size, void* userContextCallback);
static void reportStatusCallback(int result, void* context);
// **************************************** //

int main(void)
{
    struct timespec Wait = { .tv_sec = 1, .tv_nsec = 0 };
 
    // IoT Hubのクライアントハンドルを作成し、memsetで初期化
    static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClient;
    memset(&iothubClient, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));

    if (AzureIoTHubSetup(&iothubClient) == false) {
        return -1;
    }

    // "desired"プロパティが更新された際に通知を受け取り、どのコールバックを実行するかの設定
    IoTHubDeviceClient_LL_SetDeviceTwinCallback(iothubClient, twinCallback, NULL);

    /************* reported用の処理 *************/
    // reportedプロパティのためにjsonオブジェクトを作成
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);
    // reportedは "Prefecture" = "Shimane"を新規に追加するように指定
    (void)json_object_dotset_string(root_object, "Prefecture", "Shimane");
    char* reportstr = json_serialize_to_string(root_value);
    json_value_free(root_value);
    
    // SendReportedStateによってreportedプロパティを更新
    IoTHubDeviceClient_LL_SendReportedState(iothubClient, (const unsigned char*)reportstr, strlen(reportstr), reportStatusCallback, NULL);
    /**********************************************/

    while (true) {
        Log_Debug("Running...\n");

        IoTHubDeviceClient_LL_DoWork(iothubClient);

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
/// IoT Hubから"Desired"プロパティ更新がインボークされた際にコールバックとして実行される
/// </summary>
/// <param name="updateState"></param>
/// <param name="payLoad"></param>
/// <param name="payLoadSize"></param>
/// <param name="userContextCallback"></param>
static void twinCallback(DEVICE_TWIN_UPDATE_STATE updateState, const unsigned char* payLoad,
	size_t payLoadSize, void* userContextCallback)
{
    // payLoadには、IoT Hubとの初回同期時には"desired"と"reported"両方のタグが格納される
    // 例) PayLoad: {"desired":{"telemetryConfig":{},"test":"145","myval":2,"$version":29},"reported":{"$version":1}}
    // 2回目以降は、desiredタグ内のプロパティのみが格納される。"desired"タグがない点に注意
    // 例) {"test":"145","myval":5,"$version":30}
    // システムプロパティには"$"シンボルが付与され、どうもそれはここに届いていない

    // 念のためペイロードのサイズを調整する
    char recvJson[payLoadSize];
    strncpy(recvJson, payLoad, payLoadSize);

    JSON_Value* twinInfo = json_parse_string(recvJson);
    if (twinInfo == NULL) {
        Log_Debug("Error at json_parse_string\n");
    }

    JSON_Object* twinObj = json_value_get_object(twinInfo);
    // 2回目以降は"desired"タグはなくなり、直接"desired"タグ内の各タグへアクセスする
    int myval = json_object_dotget_number(twinObj, "myval");
    Log_Debug("newValue is %d\n", myval);

}

static void reportStatusCallback(int status, void* userContextCallback) {

    Log_Debug("Device Twin reported properties result: %d\n", status);
}
#include <applibs/log.h>
#include <applibs/gpio.h>
#include "iothub_device_client_ll.h"
#include "iothub_security_factory.h"
#include "iothubtransportmqtt.h"
#include "hw/template_appliance.h"

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#define hubHostName "w-iothub-azsphere.azure-devices.net"
#define ConnectionStr "HostName=w-iothub-azsphere.azure-devices.net;DeviceId=AzMT3620;SharedAccessKey=EKGzxjEFs0u0NlMmoRW/d7eZNR+xWN6DOjVmEJCd7Wk="
#define deviceId "AzMT3620"

// *************** 関数宣言 *************** //
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);
static void* GetHeapMessage(const char*, size_t, ...);
static int DirectMethodCall(const char*, const char*, size_t, char**, size_t*);
// **************************************** //

// ファン状態
enum FanStatus {
    Active,
    Stop,
    Fail
};

int main(void)
{
    struct timespec Wait = { .tv_sec = 1, .tv_nsec = 0/*5000000000*/ };

    // IoT Hubのクライアントハンドルを作成し、memsetで初期化
    static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClient;
    memset(&iothubClient, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));

    if (AzureIoTHubSetup(&iothubClient) == false) {
        return -1;
    }
    
    // "DirectMethodCall"関数をDirect Methodが実行された時の
    // コールバック関数として登録
    IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClient, DirectMethodCall, NULL);

    while (true) {
        Log_Debug("running...\n");

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
/// バックエンドアプリまたはIoT Hubによってダイレクトメソッドがインボークされた際に実行される
/// </summary>
/// <param name="methodName"></param>
/// <param name="payload"></param>
/// <param name="payloadSize"></param>
/// <param name="responsePayload"></param>
/// <param name="responsePayloadSize"></param>
/// <returns></returns>
static int DirectMethodCall(const char* methodName, const char* payload, size_t payloadSize, char** responsePayload, size_t* responsePayloadSize)
{
    *responsePayload = NULL;  // Reponse payload content.
    *responsePayloadSize = 0; // Response payload content size.

    Log_Debug("\nDirect Method Called %s with payload %s\n", methodName, payload);
    int result = 202; // リクエストは受理されたが、まだ実行されていない

    if (strcmp(methodName, "FANon") == 0) {
        Log_Debug("FANon Method Called\n");
        result = 200;
        static const char resetOkResponse[] =
            "{ \"success\" : true, \"message\" : \"%s\" }";
        *responsePayload = GetHeapMessage(resetOkResponse, sizeof(resetOkResponse), "ON");

        *responsePayloadSize = strlen(*responsePayload);
    }
    return result;

}


/// <summary>
/// ダイレクトメソッドを呼び出したアプリへ返すためのレスポンス文字列を生成する
/// </summary>
/// <param name="mssgFormat">フォーマット指定子を含む文字列</param>
/// <param name="maxlength">最大文字列長</param>
/// <param name="指定子リスト">フォーマット指定子に対応する値</param>
/// <returns></returns>
static void* GetHeapMessage(const char* mssgFormat, size_t maxlength, ...) {

    va_list args;
    // maxlengthより後ろの引数をすべて "args" リストへ格納
    va_start(args, maxlength);

    // responsePayloadにはヒープ領域を使用し、Azure IoT Hub SDKが解放の責任を負う
    char* heapMessage = malloc(maxlength);

    // メッセージフォーマットには複数の変数を表示させる必要がある可能性がある
    // そのため "va_list" を使用してまとめて引数を渡す
    vsnprintf(heapMessage, maxlength, mssgFormat, args);
    
    va_end(args);

    return heapMessage;
}
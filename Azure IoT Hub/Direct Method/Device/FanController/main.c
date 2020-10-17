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

#define hubHostName "xxx.azure-devices.net"
#define ConnectionStr "HostName=xxx"
#define deviceId "xxx"

// ファン状態
enum FanStatus {
    Active,
    Stop,
    Fail
};

// *************** 関数宣言 *************** //
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);
static void* GetHeapMessage(const char*, size_t, ...);
static int DirectMethodCall(const char*, const char*, size_t, char**, size_t*, void*);

            /***DIO制御用関数***/
int InitializeDIO(int);
int SetDIO(int, enum FanStatus);
// **************************************** //

int main(void)
{
    struct timespec Wait = { .tv_sec = 0, .tv_nsec = 8000000000 };
    // DIOを初期化
    int fdptr = InitializeDIO(FanPort);

    // IoT Hubのクライアントハンドルを作成し、memsetで初期化
    static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClient;
    memset(&iothubClient, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));

    if (AzureIoTHubSetup(&iothubClient) == false) {
        return -1;
    }
    
    // "DirectMethodCall"関数をDirect Methodが実行された時のコールバック関数として登録
    // 引数にDIOへのファイルディスクリプタを渡す
    IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClient, DirectMethodCall, fdptr);

    while (true) {
        Log_Debug(".");

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
static int DirectMethodCall(const char* methodName, const char* payload, size_t payloadSize, char** responsePayload, size_t* responsePayloadSize, void* userContextCallback)
{
    *responsePayload = NULL;  // Reponse payload content.
    *responsePayloadSize = 0; // Response payload content size.

    int diofd = (int)userContextCallback;
    int result = 202; // リクエストは受理されたが、まだ実行されていない

    if (strcmp(methodName, "FANon") == 0) {
        Log_Debug("FANon Method Called\n");
        SetDIO(diofd, Active);
        static const char resetOkResponse[] =
            "{ \"success\" : true, \"message\" : \"%s\" }";

        // "ON"文字列をレスポンスに渡すため、"resetOkResponse"に文字列に +2文字
        *responsePayload = GetHeapMessage(resetOkResponse, sizeof(resetOkResponse) + 2, "ON");
        *responsePayloadSize = strlen(*responsePayload);

        result = 200;
    }
    else if (strcmp(methodName, "FANoff") == 0) {
        Log_Debug("FANoff Method Called\n");
        SetDIO(diofd, Stop);
        static const char resetOkResponse[] =
            "{ \"success\" : true, \"message\" : \"%s\" }";

        // "OFF"文字列をレスポンスに渡すため、"resetOkResponse"に文字列に +3文字
        *responsePayload = GetHeapMessage(resetOkResponse, sizeof(resetOkResponse) + 3, "OFF");
        *responsePayloadSize = strlen(*responsePayload);

        result = 200;
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


/// <summary>
/// DIO出力ポートを初期化する
/// </summary>
/// <param name="gpioId">DIOポート</param>
/// <returns>GPIOへのfdポインタ</returns>
int InitializeDIO(int gpioId) {
    int LED = GPIO_OpenAsOutput(gpioId, GPIO_OutputMode_PushPull, GPIO_Value_High);
    if (LED == -1) {
        return NULL;
    }

    return LED;
}


/// <summary>
/// DIO出力を設定する
/// </summary>
/// <param name="gpiofd"></param>
/// <param name=""></param>
/// <returns></returns>
int SetDIO(int gpiofd, enum FanStatus status) {
    if (status == Active) {
        GPIO_SetValue(gpiofd, GPIO_Value_Low);
    }
    else if (status == Stop) {
        GPIO_SetValue(gpiofd, GPIO_Value_High);
    }

    return gpiofd;
}
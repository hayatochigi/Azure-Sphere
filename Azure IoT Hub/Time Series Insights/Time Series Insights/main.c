#include <applibs/log.h>
#include <applibs/adc.h>
#include "iothub_device_client_ll.h"
#include "iothubtransportmqtt.h"

#include "parson.h"
#include "hw/hardware_definition.h"

#include <stdbool.h>
#include <time.h>

#define ConnectionStr "HostName=xxx"
#define RefVolt 2.5
#define sampMaxVolt 2.5

// *************** 関数宣言 *************** //
bool AzureIoTHubSetup(IOTHUB_DEVICE_CLIENT_LL_HANDLE*);
bool AzureIoTHubSend(IOTHUB_DEVICE_CLIENT_LL_HANDLE*, char*);
char* CreateTelemetryMssg(int);
// **************************************** //

// *****  Global ***** //
static int BitDepth;
// ******************* //

int main(void)
{
    struct timespec Wait = { .tv_sec = 0, .tv_nsec = 5000000000 };

    // IoT Hubのクライアントハンドルを作成し、memsetで初期化
    static IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClient;
    memset(&iothubClient, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));

    if (AzureIoTHubSetup(&iothubClient) == false) {
        return -1;
    }

    // ADCのセットアップ ここから
    int adcfd = ADC_Open(ADC);
    if (adcfd == -1) {
        Log_Debug("Error at ADC_Open\n");
        return errno;
    }
    BitDepth = ADC_GetSampleBitCount(adcfd, AI0);
    if (ADC_SetReferenceVoltage(adcfd, 0, 2.5) == -1) {
        Log_Debug("Error at ADC_SetReferenceVoltage\n");
    }
    // ADCのセットアップ ここまで

    while (true) {

        if (AzureIoTHubSend(&iothubClient, CreateTelemetryMssg(adcfd)) == false) {
            return -1;
        }
        nanosleep(&Wait, NULL);
    }

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
    // メッセージにプロパティを追加
    IoTHubMessage_SetProperty(myMessage, "PathType", "Hot");

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
/// IoT Hubへのテレメトリメッセージを作成する
/// </summary>
/// <param name="client"></param>
/// <returns></returns>
char* CreateTelemetryMssg(int ADC_FileDes) {

    // JSON形式でメッセージを作成
    JSON_Value* root_value = json_value_init_object();
    JSON_Object* root_object = json_value_get_object(root_value);

    // ADCからの読み取り値を電圧 -> 文字列へ変換
    uint32_t value = 0;
    char buf[10];

    // ADC値を取得
    ADC_Poll(ADC_FileDes, 0, &value);
    float voltage = ((float)value * sampMaxVolt) / (float)((1 << BitDepth) - 1);

    // JSONオブジェクトに電圧情報を追加
    snprintf(buf, sizeof(buf), "%f", voltage);
    (void)json_object_dotset_string(root_object, "myVolt", buf);
    // JSONオブジェクトに電流情報を追加
    snprintf(buf, sizeof(buf), "%f", voltage * 2.5);
    (void)json_object_dotset_string(root_object, "myCurr", buf);

    char* json_mssg = json_serialize_to_string(root_value);
    json_value_free(root_value);

    return json_mssg;
}
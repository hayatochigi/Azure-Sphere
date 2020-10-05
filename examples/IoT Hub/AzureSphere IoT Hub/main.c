#include <applibs/log.h>
#include <time.h>

#include "azureiot/iothub_device_client_ll.h"
#include "azureiot/iothubtransportmqtt.h"
#include "applibs/networking.h"
#include <iothub_security_factory.h>
#include <azure_sphere_provisioning.h>

// DPSのスコープIDをportalの概要欄から確認して記載
#define scopeId "xxxxxx"

int ErrorCheck(IOTHUB_CLIENT_RESULT, char*);

int main(void)
{
    // 関数の戻り値保持用のresult関数
    IOTHUB_CLIENT_RESULT clientResult = 0;

    // 連続送信の周期制御時間
    // DoWorkの関数ヘルプを読むと、100m周期が推奨されている
    struct timespec Timer_5sec = { .tv_sec = 5, .tv_nsec = 0 };

    // まずはネットワークチェック
    // ここでNGならネットワーク設定からやり直してね
    bool isNetworkReady = false;
    if (Networking_IsNetworkingReady(&isNetworkReady) == -1 || !isNetworkReady) {
        Log_Debug("Network is not Ready. Try \"azsphere device wifi add\" command from cmd.\r\n");
        return -1;
    }

    // クライアントハンドルを定義してmemsetで領域を確保
    IOTHUB_DEVICE_CLIENT_LL_HANDLE IoTClientHandle = NULL;
    memset(&IoTClientHandle, 0, sizeof(IOTHUB_DEVICE_CLIENT_LL_HANDLE));

    // DPS経由でIoTHubへの接続を行う
    AZURE_SPHERE_PROV_RETURN_VALUE provResult = 
        IoTHubDeviceClient_LL_CreateWithAzureSphereDeviceAuthProvisioning(scopeId, 10000, &IoTClientHandle);
    if (provResult.result != AZURE_SPHERE_PROV_RESULT_OK) {
        Log_Debug("Failed at CreateWithAzureSphereDeviceAuthProvisioning");
    }

    while(true){

        // 送信メッセージを作成
        IOTHUB_MESSAGE_HANDLE myMessage = IoTHubMessage_CreateFromString("Hello, World");

        // クライアントハンドルに、送信するメッセージを追加
        // この時点で送信が行われるのではなく、実際にはDoWorkで送信が実行される
        clientResult = IoTHubDeviceClient_LL_SendEventAsync(IoTClientHandle, myMessage, NULL, NULL);
        ErrorCheck(clientResult, "SendEventAsync");

        // メッセージを追加したら、そのメッセージハンドル自体は不要のため破棄
        IoTHubMessage_Destroy(myMessage);        

        // 実行する
        IoTHubDeviceClient_LL_DoWork(IoTClientHandle);

        // ループ周期制御
        nanosleep(&Timer_5sec, NULL);
        }

    // クライアントハンドルを破棄
   IoTHubDeviceClient_LL_Destroy(IoTClientHandle);
    

    return 0;
}

/// <summary>
/// エラー情報を吐き出す関数
/// </summary>
/// <param name="result">関数からの戻り値</param>
/// <param name="funcName">関数名</param>
/// <returns></returns>
int ErrorCheck(IOTHUB_CLIENT_RESULT result, char* funcName) {
    if (result != IOTHUB_CLIENT_OK) {
        Log_Debug("Error occured at: %s\n", funcName);
    }
}

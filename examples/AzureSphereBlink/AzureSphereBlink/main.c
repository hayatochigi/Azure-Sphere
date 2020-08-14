#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

// Azure Sphereは以下の非同期イベントをサポートする。
// https://docs.microsoft.com/ja-jp/azure-sphere/app-development/threads-concurrency
// このプログラムではpthreadを使用して、3つのLEDを異なるタイミングで制御する。
#include <pthread.h>

#pragma region comment for header files

// ソリューションエクスプローラから、
// AzureSphereBlink > AzureSphereBlink > HardwareDefinitions > mt3630_rdb > inc > hw
// に"hw/template_appliance.h"のヘッダファイルが存在する。
// ヘッダファイルはjsonファイルから、以下のコマンドを用いて自動生成する。
// azsphere hardware-definition generate-header --input <filename>
// https://docs.microsoft.com/ja-jp/azure-sphere/hardware/hardware-abstraction

// jsonファイルにI/Oを追加した場合、アプリケーションマニフェストの Capabilities セクションに
// 必要なリソースを列記して、機能使用をオプトインする必要がある。
// Azure Sphere のアプリケーション マニフェスト https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest

// 例えば複数のGpioを定義する場合、Gpioの記述は1行にまとめて以下のように書く。
// "Gpio": [ "$LED1_RED", "$LED2_GREEN", "$LED3_BLUE" ],
// ※ "Gpio": ["$xxx"]を1行ずつ書いていたら、最後の定義しか適用されないようで、ほかのLEDが動作しなかった。。。

#pragma endregion

#include <hw/template_appliance.h>
// ミリ秒表記をナノ秒表記へ変換するための係数
#define ms2ns 1000000

// LEDのピンアサインなどはこちらを参照
// MT3620 開発ボード (RDB) ユーザー ガイド https://docs.microsoft.com/ja-jp/azure-sphere/hardware/mt3620-user-guide
// 定義jsonファイルはこちら C:\Program Files (x86)\Microsoft Azure Sphere SDK\HardwareDefinitions\mt3620_rdb.json

/// <summary>
/// Exit codes for this application. These are used for the
/// application exit code. They must all be between zero and 255,
/// where zero is reserved for successful termination.
/// </summary>
typedef enum {
    ExitCode_Success = 0,
    ExitCode_Main_Led = 1,
    Thread_CreationError = 123,
    Thread_JoinError = 124    
} ExitCode;

/// <summary>
/// LEDコントローラへ渡すデータ構造体
/// <param name="LEDport">GPIO_OpenAsOutput関数で定義したLED</param>
/// <param name="span">点滅間隔 (ミリ秒)</param>
/// </summary>
struct LED_data {
    int LEDport;
    long span;
};
void LED_Controller(LED_data);

int main(void)
{
    //並列実行用のスレッドとリファレンスを作成
    pthread_t thread_RED,thread_GREEN,thread_BLUE;
    int ret_RED,ret_GREEN,ret_BLUE;
    
    // LEDの出力設定を行う。出力プッシュプルモードで初期値はHigh (High = LED OFF状態)
    int LED1 = GPIO_OpenAsOutput(LED1_RED, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    int LED2 = GPIO_OpenAsOutput(LED2_GREEN, GPIO_OutputMode_PushPull, GPIO_Value_Low);
    int LED3 = GPIO_OpenAsOutput(LED3_BLUE, GPIO_OutputMode_PushPull, GPIO_Value_High);

#pragma region Define Data to LED Controller
    // LEDコントローラーへ渡すデータを定義
    struct LED_data RED_data;
    RED_data.LEDport = LED1;
    RED_data.span = 200;

    struct LED_data GREEN_data;
    GREEN_data.LEDport = LED2;
    GREEN_data.span = 400;

    struct LED_data BLUE_data;
    BLUE_data.LEDport = LED3;
    BLUE_data.span = 800;
#pragma endregion    

    // LECコントローラへ複数の引数を渡すために、構造体のポインタを使用する
    // 実際のアプリケーションではスレッドの起動確認のため、エラー処理を実装する
    ret_RED = pthread_create(&thread_RED, NULL,(void *)LED_Controller,(void *)&RED_data);
    ret_GREEN = pthread_create(&thread_GREEN, NULL, (void*)LED_Controller, (void*)&GREEN_data);
    ret_BLUE = pthread_create(&thread_BLUE, NULL, (void*)LED_Controller, (void*)&BLUE_data);

    ret_RED = pthread_join(thread_RED, NULL);
    ret_GREEN = pthread_join(thread_GREEN, NULL);
    ret_BLUE = pthread_join(thread_BLUE, NULL);
  
    /* エラー処理は本当は必要だが、実際のアプリケーションとしてこのプロジェクトは使用しないため割愛
    if (fd < 0) {
        Log_Debug(
            "Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
            strerror(errno), errno);
        return ExitCode_Main_Led;
    }
    */
}

/// <summary>
/// 引数で指定したLEDに対する点滅操作を行います。
/// 引数は構造体のポインタで渡されます。
/// </summary>
void LED_Controller(struct LED_data *arg){
    struct timespec sleepTime = { .tv_sec = 0, .tv_nsec = arg->span * ms2ns };
    while (true) {
        GPIO_SetValue(arg->LEDport, GPIO_Value_Low);
        nanosleep(&sleepTime, NULL);
        GPIO_SetValue(arg->LEDport, GPIO_Value_High);
        nanosleep(&sleepTime, NULL);
    }
}

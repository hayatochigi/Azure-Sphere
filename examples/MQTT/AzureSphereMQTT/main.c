#include <applibs/log.h>
#include <stdio.h>
#include <time.h>
#include "applibs/networking.h"
#include "mqtt.h"
#include "posix_sockets.h"

#define address "mqtt.thingspeak.com"
#define port "1883"
#define username "knownuser"        // ユーザー名は一意のもの
#define password "xxxxxY8V68Wxxxxx" //パスワードとしてMQTT APIキーを使用する

int main(void)
{
    // まずはネットワークチェック
    // ここでNGならネットワーク設定からやり直してね
    bool isNetworkReady = false;
    if (Networking_IsNetworkingReady(&isNetworkReady) == -1 || !isNetworkReady) {
        Log_Debug("Network is not Ready. Try \"azsphere device wifi add\" command from cmd.\r\n");
        return -1;
    }

    // non-blocking TCPソケットを作成。アプリケーションマニフェストでBrokerを
    // "AllowedConnections"しないと"Try Again"エラー発生
    int sockfd = open_nb_socket(address, port);
    if (sockfd == -1) {
        Log_Debug("Error at open_nb_socket\n");
    }


    // MQTT-Cを使用するためには必ず最初に
    // "mqtt_init"関数を呼ぶ必要がある。
    // このサンプルはpublish操作のみのため、CallbackはNULLにしている
    struct mqtt_client client;
    uint8_t sendbuf[2048];
    uint8_t recvbuf[2048];
    mqtt_init(&client, sockfd, sendbuf, sizeof(sendbuf), recvbuf, sizeof(recvbuf), NULL);
    if (client.error != MQTT_OK) {
        Log_Debug("Error at mqtt_init\n");
    }

    // MQTT Brokerへの接続を開始
    const char* client_id = NULL;
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;
    mqtt_connect(&client, client_id, NULL, NULL, 0, username, password, connect_flags, 400);
    if (client.error != MQTT_OK) {
        Log_Debug("Error at mqtt_connect\n");
    }
    else {
        Log_Debug("Connection established...\n");
    }


    // パブリッシュ
    // Publish to a Channel Field Feed
    // https://jp.mathworks.com/help/thingspeak/publishtoachannelfieldfeed.html
    // topic: channels/<channelID>/publish/fields/field<fieldnumber>/<apikey>
    // value: チャネルの指定フィールドに入力するデータ。文字列として指定します

    // タイミング制御用のtimespec
    struct timespec Timer_5sec = { .tv_sec = 5, .tv_nsec = 0 };
    // トピックの記述はヘルプを参照
    char* topic = "channels/1180450/publish/DICJW6N7UUG8RNQZ";
    char* field_val[255];
    int counter = 0;

    while(1){
        // ヘルプに従い、2つのフィールド値を"&"で区切って指定
        snprintf(field_val, sizeof(field_val), "field1=%d&field2=%d", counter, counter * 10);
      
        // ThingSpeak BrokerはQOS "0"を使用するようマニュアルに記述あり
        mqtt_publish(&client, topic, field_val, strlen(field_val), MQTT_PUBLISH_QOS_0);
        if (client.error != MQTT_OK) {
            Log_Debug("Error at mqtt_publish\n");
        }

        // 実際のsendは"mqtt_sync"で行われることに注意
        //mqtt_sync(&client);

        // 単純なループ周期制御
        nanosleep(&Timer_5sec, NULL);
        counter++;
    }

    mqtt_disconnect(&client);


    return 0;
}

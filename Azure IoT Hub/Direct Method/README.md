# Direct Method
このサンプルではAzure Sphere MT3620のLEDをON/OFFすることでファンのON/OFF操作を模擬する。IoT Hubからダイレクトメソッドを呼び出し、Azure Sphere上の制御関数を実行する(そのうちC#のバックエンドアプリを作る)。

[IoT Hub からのダイレクト メソッドの呼び出しについて](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-direct-methods)  
[ダイレクト メソッドを呼び出すコードを記述する](https://docs.microsoft.com/ja-jp/learn/modules/remotely-monitor-devices-with-azure-iot-hub/5-write-code-direct-methods)


「やろうと思えばcloud-to-deviceでもできるのでは？」と思う。多分そうなんだけど、ダイレクトメソッドはメソッドをペイロードを付与して実行し、そもそも実行できたかどうか、実行したならその結果をペイロードとして受け取る機構がすでに組み込まれている。実行の結果がすぐにわかり、失敗した場合はすぐに対処できるというメリットがある。

デバイスツインも似たような機構を持っているが、これはちょっと違う。デバイスツインは通信が繋がったらメッセージを送るので、いつそれが届くかちょっと保証がない。。。ダイレクトメソッドはすぐに実行されるのでその点で、ダイレクトメソッドは緊急時などに有用な手法かなと思う。

* cloud-to-device  
IoT Hubからメッセージを送れる。送信に対するデバイスの結果はデバイス側で実装が必要

* ダイレクトメソッド  
IoT Hubからメッセージと共に、コマンドの実行命令を送れる

* デバイスツイン  
IoT Hubから"Desired"プロパティを用いてメッセージを送れる。ほかにもいろいろできる

## ダイレクトメソッド実装の流れ
1. ダイレクトメソッドで呼び出す関数を定義する
```
static int DirectMethodCall(const char* methodName, const char* payload, size_t payloadSize, char** responsePayload, size_t* responsePayloadSize, void* userContextCallback)
```
"payload"はIoT HubからのメッセージをJSON形式で、"responsePayload"はIoT HubへのメッセージをJSON形式で。  
"responsePayload"はヒープメモリを作成して使用する。解放はAzure IoT Hub SDKが行う

2. DirectMethodCall内で関数を定義  
"methodName"に基づいて、実行する関数を定義する

3. 登録する  
[IoTHubDeviceClient_LL_SetDeviceMethodCallback](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-c-sdk-ref/iothub-device-client-ll-h/iothubdeviceclient-ll-setdevicemethodcallback)を使用して、ダイレクトメソッドが呼び出された際の関数を定義する。
```
IoTHubDeviceClient_LL_SetDeviceMethodCallback(iothubClient, DirectMethodCall, NULL);
```

4. ダイレクトメソッドを呼び出す  
ここではPortal上から呼び出しているが、バックエンドアプリを別に作成して呼び出すことも可能。Portalから実行すると、ON/OFFのコマンドに応じてMT3620上のLEDが点灯/消灯し、それぞれ以下のようにデバイスからのレスポンスを確認できる。

<p align="center">
  <img width="240" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/DirectMethod.PNG">
</p>


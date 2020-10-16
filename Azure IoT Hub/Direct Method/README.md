# Direct Method
このサンプルではAzure Sphere MT3620のLEDをON/OFFすることでファンのON/OFF操作を模擬する。C#で開発したバックエンドアプリを用いて、Azure Sphere上の制御関数を実行する。
バックエンドアプリはファンのON/OFFを制御するが、このサンプルは稀に制御が失敗するように作る。実際のバックエンドアプリではエラー応答に対して明確に対応を行う必要がある。

[IoT Hub からのダイレクト メソッドの呼び出しについて](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-direct-methods)  
[ダイレクト メソッドを呼び出すコードを記述する](https://docs.microsoft.com/ja-jp/learn/modules/remotely-monitor-devices-with-azure-iot-hub/5-write-code-direct-methods)

# Direct Method
このサンプルではAzure Sphere MT3620のLEDをON/OFFすることでファンのON/OFF操作を模擬する。IoT Hubからダイレクトメソッドを呼び出し、Azure Sphere上の制御関数を実行する(そのうちC#のバックエンドアプリを作る)。

[IoT Hub からのダイレクト メソッドの呼び出しについて](https://docs.microsoft.com/ja-jp/azure/iot-hub/iot-hub-devguide-direct-methods)  
[ダイレクト メソッドを呼び出すコードを記述する](https://docs.microsoft.com/ja-jp/learn/modules/remotely-monitor-devices-with-azure-iot-hub/5-write-code-direct-methods)


「やろうと思えばcloud-to-deviceでもできるんでは？」と思う。多分そうなんだけど、ダイレクトメソッドはメソッドを実行し、その結果を受け取る機構が組み込まれている。実行の結果がすぐにわかり、失敗した場合はすぐに対処できるというメリットがある。デバイスツインも

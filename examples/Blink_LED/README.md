# AzureSphere Blink Example

このサンプルプログラムは、Visual StudioのAzure Sphereテンプレートを改修したもので、
Azure Sphere MT3620開発キットに搭載された3つのLEDを、それぞれ異なる色・タイミングで点滅させる。
LEDのピンアサインはこちら。
[MT3620 参照開発ボード (RDB) のユーザー ガイド](https://docs.microsoft.com/ja-jp/azure-sphere/hardware/mt3620-user-guide)

このサンプルでは、pthreadを使用して福栖のスレッドを生成し、
スレッド毎にLED制御を行っている。

[非同期イベント、ポーリング、スレッド、同時実行](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/threads-concurrency)

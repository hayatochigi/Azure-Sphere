# AzureSphereBlinkサンプルについて
このサンプルプログラムは、Visual StudioのAzure Sphereテンプレートを改修したものです。Azure Sphere MT3620開発キットに搭載された3つのLEDを、それぞれ異なる色・タイミングで点滅させます。LEDのピンアサインはこちらを参照します。
[MT3620 参照開発ボード (RDB) のユーザー ガイド](https://docs.microsoft.com/ja-jp/azure-sphere/hardware/mt3620-user-guide)

このサンプルでは、
- azsphereコマンドを用いて、jsonファイルからヘッダファイルを自動生成する
- アプリケーションマニフェストへjson形式でI/Oを追加する

方法についても、main.cファイルのコメントで説明しています。

---

Azure Sphereはpthreadをサポートしています。このサンプルでは、pthreadを使用して複数のスレッドを生成し、スレッド毎にLED制御を割り当てています。

[非同期イベント、ポーリング、スレッド、同時実行](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/threads-concurrency)

# Azure Sphere TCP Example
このサンプルはAzure SphereをTCPサーバーとして、Windowsクライアントとの間でTCPによるデータの送受信を行う

Azure SphereでTCPの受信を許可するためには、"AllowedTcpServerPorts"でポートへの受信トラフィックを許可する必要がある。
同様にクライアントPCのIPアドレスを、"AllowedConnections"で登録する必要がある。

[アプリケーション マニフェスト
](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest)
```
"Capabilities": {
    "AllowedApplicationConnections": [],
    "AllowedConnections": [ "192.168.11.57" ],
    "AllowedTcpServerPorts": [ 61551]
  },
```

TCPクライアントアプリには、NonSoft様のTCP/IPテストツールを使用した。シンプルで使いやすい!

[TCP/IPテストツール](http://nonsoft.la.coocan.jp/Download/SocketTool/index.html)

<p align="center">
  <img width="500" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/TCP-result.png">
</p>

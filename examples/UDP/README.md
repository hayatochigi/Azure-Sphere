# Azure Sphere UDP Example
このサンプルは、Azure SphereとWindowsの間でUDPによるデータの送受信を行う

Azure SphereでUDPの受信を許可するためには、"AllowedUdpServerPorts"でポートへの受信トラフィックを許可する必要がある。
同様に送信の場合は、"AllowedConnections"で送信先のPCを登録する必要がある。

[アプリケーション マニフェスト
](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest)
```
"Capabilities": {
    "AllowedConnections" : [ "192.168.1.2" ],
    "AllowedUdpServerPorts": [ 1024, 50000 ]
  },
```

このサンプルでは、Windows側のUDPアプリケーションにLabVIEWを用いた。

- Azure Sphere -- ("ABC") --> Windows
- Windows -- ("EFG") --> Azure Sphere

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/UDP-result.PNG">
</p>

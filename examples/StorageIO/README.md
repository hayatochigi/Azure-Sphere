# Azure Sphere Storage Example
このサンプルは、Azure Sphereの変更可能ストレージを構成して、ファイルの作成/書き込み/読み取り/閉じるの一連の流れを行う。

[Azure Sphere でのストレージの使用](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/storage)

Azure Sphereの変更可能ストレージを使用するためには、アプリケーションマニフェストに以下の記述を行う。

[アプリケーション マニフェスト
](https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest)
```
"Capabilities": {
    "AllowedApplicationConnections": [],
    "MutableStorage": { "SizeKB": 8 }
  },
```

ストレージ使用のヘルプにも記載ある通り、この変更可能ストレージはアプリケーションのコンポーネントIDに紐づけられていて、ほかのアプリケーションからはアクセスできない点に注意。

```
azsphere device app show-quota
```
コマンドで確認すると、コンポーネントIDとストレージが紐づけられている様子を確認できる。このサンプルを実行して、プログラム最終行にあるストレージの削除関数を実行しない場合は、下図のようにストレージの使用量変化を確認できる。

<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/show-quota.png">
</p>

## 実行結果
<p align="center">
  <img width="800" src="https://github.com/hayatochigi/images/blob/master/Azure%20Sphere%20Example/storage-result.PNG">
</p>


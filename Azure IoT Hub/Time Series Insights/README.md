# Time Series Insights(TSI) with Azure Sphere
[Time Series Insights](https://docs.microsoft.com/ja-jp/azure/time-series-insights/overview-what-is-tsi)は、IoTデバイスからのデータを収集し、クエリや視覚化を簡単に行える分析サービス。このサンプルでは、Azure Sphereから送信した2種類のテレメトリをTSIで可視化する。データを可視化するアプリはよくあるけど、ネットワークやスケーラビリティを考慮して構築するのは結構骨が折れる。。。TSIを活用すると、データからのインサイトを素早く引き出し、アクションにつなげることができる。

TSIは時系列でデータを整理するので、デバイスからのテレメトリには時間情報が含まれている必要がある(のだろう...なかったらどうなるかまで試してない)。Azure SDKを使っている限りは必ずテレメトリに時間情報が付与されるので、たいてい気にしなくて良いと思ってる。

<p align="center">
  <img width="600" src="https://github.com/hayatochigi/images/blob/master/IoT%20Hub%20Example/TSI.PNG">
</p>

サンプルといっても、やっていることは他のAzure Sphereサンプルと変わらなくて、2種類のデータをJSON形式で送っているに過ぎない。


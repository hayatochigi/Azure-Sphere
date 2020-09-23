#include <applibs/log.h>
#include "applibs/networking.h"

#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

// 受信の場合はアプリケーションマニフェストの"AllowedTcpServerPorts"に含める必要がある
#define serverPort 61551 //Azure Sphere上のTCPサーバーポート

// クライアントIPアドレスはアプリケーションマニフェストの"AllowedConnections"で定義する

int CreateSocket();

int main(void)
{
    int result = 0;

    // まずはネットワークチェック
    // ここでNGならネットワーク設定からやり直してね
    bool isNetworkReady = false;
    if (Networking_IsNetworkingReady(&isNetworkReady) == -1 || !isNetworkReady) {
        Log_Debug("Network is not Ready. Try \"azsphere device wifi add\" command from cmd.\r\n");
        return -1;
    }

    // サーバーとクライアントそれぞれのソケット構造体
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));

    // ソケットを作成する。
    // サーバーのソケットとは、 "socket(2) によって生成され、 
    // bind(2) によってローカルアドレスにバインドされ、 listen(2) を経て接続を待っているソケット"
    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/accept.2.html
    int serverSocket = CreateSocket();

    // サーバー設定
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    result = bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (result < 0) {
        Log_Debug("Error at bind\n");
        return errno;
    }

    // ソケットに「接続待ちソケット」の印をつける
    // 保留中接続キューの最大長は"5"で設定
    result = listen(serverSocket, 5);
    if (result < 0) {
        Log_Debug("Error at listen\n");
        return errno;
    }

    // クライアントからソケットへの接続を受ける
    Log_Debug("Waiting for client connection\n");
    int len = sizeof(clientAddr);
    result = accept(serverSocket, (struct sockaddr*)&clientAddr, &len);
    if (result < 0) {
        Log_Debug("Error at accept\n");
        return errno;
    }
    else
    {
        // クライアントのIPアドレスを確認
        Log_Debug("connected from %s.\n", inet_ntoa(clientAddr.sin_addr));
    }

    // acceptは受け付けたソケットのディスクリプタを返す。このfdは第一引数のものと異なる
    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/accept.2.html
    if (serverSocket != result) {
        // 元のソケットfdとacceptのfdが異なる場合は元のソケットfdを破棄
        close(serverSocket);
        // 元のソケットをacceptのものと入れ替え
        serverSocket = result;
    }

    // 送信データ「ABC」を作成
    char exData[3] = { 0x41, 0x42, 0x43 };

    // データを送信する
    ssize_t sendSize = send(serverSocket, exData, sizeof(exData), MSG_NOSIGNAL);
    if (sendSize < 0) {
        Log_Debug("Error at send\n");
        return errno;
    }

    // ディスクリプタを閉じる
    close(serverSocket);

}

/// <summary>
/// 新規ソケットを作成する
/// </summary>
/// <returns>作成したソケットのディスクリプタ</returns>
int CreateSocket() {
    // 通信はIPv4(AF_INET)、TCPなのでSOCK_STREAMを選択
    // 指定できる値は、  TCP  ソケットには  0  かIPPROTO_TCP、  UDP  ソケットには  0  か  IPPROTO_UDP  に限られる。
    int newSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (newSocket < 0) {
        Log_Debug("Error at socket\n");
    }
    return newSocket;
}
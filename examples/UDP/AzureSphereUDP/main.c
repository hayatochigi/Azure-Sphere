#include <applibs/log.h>
#include "applibs/networking.h"

#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

// 受信の場合はアプリケーションマニフェストの"AllowedUdpServerPorts"に含める必要がある
#define exPort 61557 //Azure Sphereが送信する際のポート
#define inPort 61556 //Azure Sphereが受信する際のポート。マニフェストに含める

// IPアドレスはアプリケーションマニフェストの"AllowedConnections"に含める必要がある
// 含めない場合、"sendto"においてerrno = 1、"Operation not permitted"が発生する
#define IPaddress "192.168.0.110"

int CreateSocket();

int main(void)
{
    // まずはネットワークチェック
    // ここでNGならネットワーク設定からやり直してね
    bool isNetworkReady = false;
    if (Networking_IsNetworkingReady(&isNetworkReady) == -1 || !isNetworkReady) {
        Log_Debug("Network is not Ready. Try \"azsphere device wifi add\" command from cmd.\r\n");
        return -1;
    }

    // 送信設定用の構造体
    struct sockaddr_in targetInfo;


    //////////// 送信 ////////////
 
    // ソケットを作成する。
    int sendSocket = CreateSocket();

    // 送信設定
    targetInfo.sin_family = AF_INET;
    targetInfo.sin_addr.s_addr = inet_addr(IPaddress);
    
    // 送信ポートを設定
    targetInfo.sin_port = htons(exPort);

    // 送信データ「ABC」を作成
    char exData[3] = { 0x41, 0x42, 0x43 };

    // データを送信する
    ssize_t sendSize = sendto(sendSocket, exData, sizeof(exData), 0, (struct sockaddr*)&targetInfo, sizeof(targetInfo));
    if (sendSize < 1) {
        Log_Debug("Nothing Sent");
    }

    // ディスクリプタを閉じる
    close(sendSocket);


    //////////// 受信 ////////////

    int recvSocket = CreateSocket();

    // 受信ポートを設定
    targetInfo.sin_family = AF_INET;
    targetInfo.sin_port = htons(inPort);
    targetInfo.sin_addr.s_addr = INADDR_ANY;

    // 受信データバッファを準備
    char recvData[3];
    memset(recvData, 0, sizeof(recvData));

    // ソケットにaddrで指定したアドレスを割り当てる(ソケットに名前を付ける)
    bind(recvSocket, (struct sockaddr*)&targetInfo, sizeof(targetInfo));
    
    // 受信
    recv(recvSocket, recvData, sizeof(recvData), 0);
    
    Log_Debug("Receive: %s\n", recvData);

    close(recvSocket);

    return 0;
}


/// <summary>
/// 新規ソケットを作成する
/// </summary>
/// <returns>作成したソケットのディスクリプタ</returns>
int CreateSocket() {
    // 通信はIPv4(AF_INET)、UDPなのでコネクションレス、信頼性なしのSOCK_DGRAM、
    // プロトコルは1つのプロトコルのみをサポートするので(UDPのみ)、マニュアルの通り"0"を記述
    int newSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (newSocket == -1) {
        Log_Debug("Error at socket\n");
    }
    return newSocket;
}
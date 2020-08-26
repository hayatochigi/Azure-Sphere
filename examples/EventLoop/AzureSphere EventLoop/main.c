// Applibs eventloop.h
// https://docs.microsoft.com/ja-jp/azure-sphere/reference/applibs-reference/applibs-eventloop/eventloop-overview
#include <stdio.h>
#include <applibs/log.h>
#include <applibs/eventloop.h>
#include <sys/timerfd.h>
#include <errno.h>

// ミリ秒表記をナノ秒表記へ変換するための係数
#define ms2ns 1000000

int Kaomoji(void);

int main(void)
{   
    // 関数からの戻り値を受け取り、エラー判定に使用する
    int result = 0;
    // EventLoopへのポインタ
    static EventLoop *myEvent;

    // 少なくとも一方に0以外の値を設定すると、繰り返しタイマーが有効になる
    // この例では、タイマの満了後、1秒後に次のタイマカウントを開始する
    struct timespec my_interval = { .tv_sec = 5, .tv_nsec = 0 };
    // タイマー満了時間の初期値。10秒後にタイマーは満了する
    struct timespec my_value = { .tv_sec = 10, .tv_nsec = 0 };
    // itimerspecはタイマーの満了時間初期値と間隔を指定する
    struct itimerspec new_value = { .it_interval = my_interval, .it_value = my_value };

    myEvent = EventLoop_Create();
    if (myEvent == NULL) {
        Log_Debug("EventLoop_Create Error\n");
    }

    // timerfdを作成して、"myTimerfd"にタイマのファイルディスクリプタ(fd)を渡す
    int myTimerfd = timerfd_create(CLOCK_REALTIME, NULL);
    if (myTimerfd == -1) {
        Log_Debug("timerfd_create Error\n");
        return errno;
    }

    // 使うことはないけどとりあえず呼んでみる
    int eventfd = EventLoop_GetWaitDescriptor(myEvent);
    if (eventfd == -1) {
        Log_Debug("Failed EventLoop_GetWaitDescriptor\n");
        return errno;
    }
    else{
        // 成功したらイベントfdを表示してみる
        Log_Debug("EventLoop Description is %d\n", eventfd);
    }

    // 作成したEventLoopオブジェクトへのポインタ、イベントソースとなるfd、
    // EventLoop_IoEvents 列挙型で定義したI/Oのビットマスク(下記リンク参照)、CB関数へのポインタ、CTポインタ
    // https://docs.microsoft.com/ja-jp/azure-sphere/reference/applibs-reference/applibs-eventloop/enum-eventloop-ioevents
    result = EventLoop_RegisterIo(myEvent, myTimerfd,EventLoop_Input ,(void *)Kaomoji,NULL);
    if (result == NULL) {
        Log_Debug("EventLoop_RegisterIo Error\n");
        return errno;
    }

    // fd, 相対時刻タイマー(0), クロックの現在の値からの相対的な時刻, NULL
    // new_value.it_value のフィールドのうち少なくとも一方に 0 以外の値を設定すると、 タイマーが開始される。
    // https://linuxjm.osdn.jp/html/LDP_man-pages/man2/timerfd_create.2.html
    result = timerfd_settime(myTimerfd, 0, &new_value, NULL);
    if (result == -1) {
        Log_Debug("timerfd_settime Error\n");
        return errno;
    }

    // myEventを開始する。"-1"の指定により、イベントがトリガされるまで待機する
    // "true"の指定により、最初のイベントが処理されたのちにループを中断する
    // アプリケーションでは、EventLoop_Run(el, -1, false) を呼び出して、呼び出し元スレッドの制御を EventLoopに渡すことができます。
    // アプリケーションで EventLoop_Run(el, -1, true) を呼び出すと、ループはブロックされ、最初のイベントの準備ができるまで待機した後、イベントを処理して戻ります。
    result = EventLoop_Run(myEvent, -1, true);
    // 戻り値について https://docs.microsoft.com/ja-jp/azure-sphere/reference/applibs-reference/applibs-eventloop/enum-eventloop-run-result
    switch (result){
    case 0: 
        Log_Debug("EventLoop_RunFinished_Empty\n");
    case -1:
        Log_Debug("EventLoop_Run_Failed\n");
        return errno;
    }

    EventLoop_Stop(myEvent);
    EventLoop_Close(myEvent);

    return 0;
}

/// <summary>
/// テンション高めの顔文字をデバッグ文字列として表示する
/// </summary>
/// <returns></returns>
int Kaomoji() {
    
    Log_Debug("\n////////// (∈^▽ﾟ) ★☆★ //////////\n");

    return 0;
}

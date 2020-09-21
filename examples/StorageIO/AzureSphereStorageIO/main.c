// アプリケーションマニフェストにおいて、永続ストレージを許可すること
// 例: "MutableStorage" : { "SizeKB": 64, }
// https://docs.microsoft.com/ja-jp/azure-sphere/app-development/app-manifest

#include <applibs/log.h>
#include <applibs/storage.h>
#include <unistd.h>

int main(void)
{
    // ファイルディスクリプタ
    int fileFd;


    ////// 書き込み処理 ///////

    // 変更可能ストレージファイルのファイルディスクリプタを取得
    // ファイルがない場合は新規作成となる
    fileFd = Storage_OpenMutableFile();
    if (fileFd == -1) {
        Log_Debug("Error at Storage_OpenMutableFile\n");
    }

    // ファイルディスクリプタへデータを書き込む
    // unistd: write -> ssize_t write(int fd, const void *buf, size_t count); 
    // 第二引数のバッファから、最大カウントバイト(第三引数)書き込む
    // 成功した場合は書き込まれたバイト数、エラーなら-1が返される。
    
    char abc[3] = { 0x41, 0x42, 0x43 }; // ABCの文字列
    ssize_t writtenSize = write(fileFd, abc, sizeof(abc));
    if (writtenSize == -1) {
        Log_Debug("Error at write\n");
    }
    else if (writtenSize == sizeof(abc)){
        Log_Debug("Write: Data size = %d.\n", sizeof(abc));
    }

    // 書き込み後に一度閉じる必要がある。
    // ファイル位置は書き込みが完了した位置になるので、
    // 閉じずに読み取ると書き込みデータを読み込めない
    close(fileFd);


    ////// 読み込み処理 ///////
    fileFd = Storage_OpenMutableFile();

    // ファイルを読み込む
    // unistd: read --> ssize_t read(int fd, void *buf, size_t count);
    // 第二引数のバッファへ最大カウントバイト(第三引数)読み込む
    // 成功した場合は読み込んだバイト数、ファイル位置はこの数だけ進められる
    //fileFd = Storage_OpenMutableFile();
    char data[3]; // 読み込みデータ格納用
    ssize_t readSize = read(fileFd, data, sizeof(data));
    if (readSize == -1) {
        Log_Debug("Error at read");
    }
    else if (readSize == sizeof(data)) {
        Log_Debug("Read : Read size = %d, data is \"%s\"\n", readSize, data);
    }
    close(fileFd);


    // (オプション) 終了時にファイルを削除する
    //Storage_DeleteMutableFile();

    return 0;
}

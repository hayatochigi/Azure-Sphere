#include <applibs/log.h>
#include <applibs/storage.h>
#include "iothub_device_client_ll.h"

#include <unistd.h>

int main(void)
{
    int fileFd;

    // 新規作成または既存ファイルを開き、fdを取得
    fileFd = Storage_OpenMutableFile();
    if (fileFd == -1) {
        Log_Debug("Error at Storage_OpenMutableFile\n");
    }

    // 書き込みデータ
    char wMessage[] = "Hi from Azure Sphere\n";
    ssize_t wSize = write(fileFd, wMessage, sizeof(wMessage));
    if (wSize == -1) {
        Log_Debug("Error at write\n");
    }
    
    close(fileFd);


    return 0;
}

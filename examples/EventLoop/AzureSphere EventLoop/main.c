#include <stdio.h>
// NULL is defined in "stdio.h"
#include <applibs/log.h>
#include <applibs/eventloop.h>
#include <sys/timerfd.h>

int LogWrite(void);

int main(void)
{
    static EventLoop *myEvent;
    struct timespec polling_t = { .tv_sec = 5, .tv_nsec = 0 };

    myEvent = EventLoop_Create();
    if (myEvent == NULL) {
        Log_Debug("Error in EventLoop_Create");
    }

    EventLoop_RegisterIo(myEvent, polling_t, LogWrite);
    

    EventLoop_Close(myEvent);

    return 0;
}

int LogWrite() {
    return 0;
}

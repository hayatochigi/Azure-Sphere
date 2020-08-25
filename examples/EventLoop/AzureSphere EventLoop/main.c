#include <stdio.h>
// NULL is defined in "stdio.h"

// Applibs eventloop.h
// https://docs.microsoft.com/ja-jp/azure-sphere/reference/applibs-reference/applibs-eventloop/eventloop-overview

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

    int EventDes = EventLoop_GetWaitDescriptor(myEvent);
    if (EventDes == -1) {
        Log_Debug("Failed EventLoop_GetWaitDescriptor");
    }
    else{
        Log_Debug("EventLoop Description is %d", EventDes);
    }

    EventLoop_RegisterIo(myEvent, NULL,(EventLoop_IoEvents)1,(void *)LogWrite,NULL);

    EventLoop_Close(myEvent);

    return 0;
}

int LogWrite() {
    
    Log_Debug("EVENT FIRED");

    return 0;
}

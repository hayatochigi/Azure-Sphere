// 高度なアプリケーションで ADC を使用する
// https://docs.microsoft.com/ja-jp/azure-sphere/app-development/adc

#include <applibs/log.h>
#include <applibs/adc.h>
#include <time.h>
#include "hw/hardware_definition.h"

#define RefVolt 2.5
#define sampMaxVolt 2.5

int InitADC(ADC_ChannelId);


int main(void)
{   
    // https://docs.microsoft.com/ja-jp/azure-sphere/reference/applibs-reference/applibs-adc/function-adc-open
    // エラーが発生した場合は、-1が返され、errnoにはエラー値が設定されます。
    int adcfd = ADC_Open(ADC);
    if (adcfd == -1) {
        Log_Debug("ADC_Open Error");
        return errno;
    }
    
    int BitDepth = ADC_GetSampleBitCount(adcfd, AI0);
    if (BitDepth == -1) {
        Log_Debug("BitDepth Error");
        return errno;
    }
    else {
        Log_Debug("Bit Depth is %d\n", BitDepth);
    }

    int result = ADC_SetReferenceVoltage(adcfd, 0, RefVolt);
    if (result == -1) {
        Log_Debug("ADC_SetReferenceVoltage Error");
        return errno;
    }

    uint32_t value = 0;
    struct timespec sleepTime = { .tv_sec = 1, .tv_nsec = 0 };

    Log_Debug("Continuous Acquisition Start\n");
    while (1) {
        result = ADC_Poll(adcfd, 0, &value);
        float voltage = ((float)value * sampMaxVolt) / (float)((1 << BitDepth) - 1);
        Log_Debug("Voltage is %.6f\n", voltage);
        nanosleep(&sleepTime, NULL);

    }
    return 0;
}


int InitADC(ADC_ChannelId adc_select) {

    return 0;
}

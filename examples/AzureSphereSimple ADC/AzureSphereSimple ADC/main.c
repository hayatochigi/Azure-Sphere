// This minimal Azure Sphere app prints "High Level Application" to the debug
// console and exits with status 0.

#include "applibs_versions.h"
#include <applibs/log.h>
#include <applibs/adc.h>
#include "hw/mt3620_rdb.h"

static int ADC_ID = -1;

int main(void)
{
    Log_Debug("High Level Application\n");
    
    
    ADC_ID = ADC_Open(MT3620_RDB_ADC_CONTROLLER0);
    if (ADC_Open != 0) {
        Log_Debug("Error occured at ADC_Open");
        return ADC_Open;
    }

    uint32_t value;
    int result = ADC_Poll(ADC_ID, MT3620_RDB_ADC_CONTROLLER0, &value);
    if (result != 0) {
        Log_Debug("Error occured at ADC_Poll");
    }
    Log_Debug("The out sample value is %.3f V\n", value);
    
    return 0;
}

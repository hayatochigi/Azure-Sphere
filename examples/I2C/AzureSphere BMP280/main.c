#include <applibs/log.h>
#include <applibs/i2c.h>
#include <unistd.h>
#include <errno.h>
#include "hw/hardware_definition.h"

#define I2C_STRUCTS_VERSION 1
#define BME280_Addr 0x76

const uint8_t config_data[] = {0xF5, 0xA0};
const uint8_t ctrl_meas[] = { 0xF4, 0x27 };
const uint8_t ctrl_hum[] = { 0xF2, 0x01 };
const uint8_t temp_reg = { 0xFE };

// BME280 – スイッチサイエンス http://trac.switch-science.com/wiki/BME280
int main(void)
{
    I2C_DeviceAddress dev_addr = BME280_Addr;
    int I2Cfd;
    I2Cfd = I2CMaster_Open(I2C_0);
    if (I2Cfd == -1) {
        Log_Debug("Error at I2C Open: %d\n", errno);
    }    

    /** Configure sensor settings**/
    if (I2CMaster_Write(I2Cfd, dev_addr, config_data, sizeof(config_data)) == -1) {
        Log_Debug("Error at config: %d\n", errno);
    }
    if (I2CMaster_Write(I2Cfd, dev_addr, ctrl_meas, sizeof(ctrl_meas)) == -1) {
        Log_Debug("Error at ctrl_meas: %d\n", errno);
    }
    if (I2CMaster_Write(I2Cfd, dev_addr, ctrl_hum, sizeof(ctrl_hum)) == -1) {
        Log_Debug("Error at ctrl_hum: %d\n", errno);
    }
    /*******************************/

    uint8_t data[8] = { 0xFA, 0xF9, 0xF7 };
    
    if (I2CMaster_Read(I2Cfd, dev_addr, data, 3) == -1) {
        Log_Debug("Error at read: %d", errno);
    }
    Log_Debug("%d, %d, %d ...\n", data[0], data[1], data[2]);

    close(I2Cfd);

    return 0;
}

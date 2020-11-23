#include <applibs/log.h>
#include <applibs/i2c.h>
#include <unistd.h>
#include <errno.h>
#include "hw/hardware_definition.h"

#define I2C_STRUCTS_VERSION 1

int main(void)
{
    int I2Cfd;
    I2Cfd = I2CMaster_Open(I2C_0);
    if (I2Cfd == -1) {
        Log_Debug("Error at I2C Open: %d\n", errno);
    }
    static const uint8_t bme280_addr = 0x76;
    char sensor_id[255];
    size_t w_size = I2CMaster_Write(I2Cfd, bme280_addr, NULL, 0);
    if (w_size == -1) {
        Log_Debug("Error at Write: %d\n", errno);
    }
    size_t r_size = I2CMaster_Read(I2Cfd, bme280_addr, sensor_id, sizeof(sensor_id));
    if (r_size == -1) {
        Log_Debug("Error at Read: %d\n", errno);
    }
    else{
        Log_Debug("sensorid: %s\n", sensor_id);
    }
    close(I2Cfd);

    return 0;
}

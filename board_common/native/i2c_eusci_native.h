#ifndef _BOARD_COMMON_NATIVE_I2C_EUSCI_H_
#define _BOARD_COMMON_NATIVE_I2C_EUSCI_H_

#include "eusci.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum i2c_data_rate {
    I2C_DATA_RATE_100KBPS = 100000,
    I2C_DATA_RATE_400KBPS = 400000,
    I2C_DATA_RATE_1MBPS = 1000000,
} i2c_data_rate_t;

typedef struct i2c {
    /**
     * Which EUSCI module this I2C is connected to
     */
    eusci_t eusci;
} i2c_t;

bool i2c_open(eusci_t eusci, i2c_data_rate_t data_rate, i2c_t * out);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_I2C_EUSCI_H_

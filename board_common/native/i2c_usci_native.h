#ifndef _BOARD_COMMON_NATIVE_I2C_USCI_H_
#define _BOARD_COMMON_NATIVE_I2C_USCI_H_

#include "usci.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct i2c {
    /**
     * Which USCI module this I2C is connected to
     */
    usci_t usci;
} i2c_t;

/**
 * Open a connection to a I2C channel
 *
 * @param usci The USCI channel to use
 * @param baud_rate The baud rate at which we will run
 * @param out The I2C structure to fill
 *
 */
//bool i2c_open(i2c_t i2c, unsigned int baud_rate, i2c_t * out);

bool i2c_open();

#ifdef __cplusplus
}
#endif

#endif // #ifndef _BOARD_COMMON_NATIVE_I2C_USCI_H_
#ifndef _BOARD_COMMON_NATIVE_I2C_H_
#define _BOARD_COMMON_NATIVE_I2C_H_

/// XXX: TODO: this should probably be exported to a common header
#include <msp430.h>
#if defined ( MSP430_CLASS_FR5xx_6xx )
#   include "msp430fr5xx_6xxgeneric.h"
#else
#   error "Unspported processor"
#endif

#include <driverlib.h>

#ifndef __MSP430_HAS_EUSCI_Bx__
#   error "We need EUSCI for I2C"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents which EUSCI module we're hooked up to
 */
typedef enum eusci {
#ifdef EUSCI_A0_BASE
    EUSCI_A0,
#endif
#ifdef EUSCI_A1_BASE
    EUSCI_A1,
#endif
#ifdef EUSCI_A2_BASE
    EUSCI_A2,
#endif
#ifdef EUSCI_A3_BASE
    EUSCI_A3,
#endif
#ifdef EUSCI_B0_BASE
    EUSCI_B0,
#endif
#ifdef EUSCI_B1_BASE
    EUSCI_B1,
#endif
#ifdef EUSCI_B2_BASE
    EUSCI_B2,
#endif
#ifdef EUSCI_B3_BASE
    EUSCI_B3,
#endif
    EUSCI_count
} eusci_t;

typedef struct i2c {
    /**
     * Which EUSCI module this I2C is connected to
     */
    eusci_t eusci;
} i2c_t;

bool i2c_open(eusci_t eusci, i2c_t * out);

// TODO: should be able to accept a slave address
void i2c_write_byte(i2c_t * channel, uint8_t byte);

// TODO: should be able to accept a slave address
void i2c_read_byte(i2c_t * channel, uint8_t byte);

void i2c_close(i2c_t channel);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_UART_H_

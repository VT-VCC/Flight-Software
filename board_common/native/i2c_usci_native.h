#ifndef _BOARD_COMMON_USCI_I2C_H_
#define _BOARD_COMMON_USCI_I2C_H_

#ifdef __cplusplus
extern "C" {
#endif

// TODO: This shuld probably be extracted to a common interface
#include <msp430.h>
#if defined (MSP430_CLASS_F5xx_6xx)
#   include "msp430f5xx_6xxgeneric.h"
#else
#   error "Unsupported processor"
#endif

#ifndef __MSP430_HAS_USCI_Bx__
#   error "We need USCI for I2C"
#endif

#include <driverlib.h>

typedef enum usci {
#ifdef USCI_A0_BASE
    USCI_A0,
#endif
#ifdef USCI_A1_BASE
    USCI_A1,
#endif
#ifdef USCI_A2_BASE
    USCI_A2,
#endif
#ifdef USCI_A3_BASE
    USCI_A3,
#endif
#ifdef USCI_B0_BASE
    USCI_B0,
#endif
#ifdef USCI_B1_BASE
    USCI_B1,
#endif
#ifdef USCI_B2_BASE
    USCI_B2,
#endif
#ifdef USCI_B3_BASE
    USCI_B3,
#endif
    USCI_count
} usci_t;

typedef struct uart {
    /**
     * Which USCI module this I2C is connected to
     */
    usci_t usci;
} uart_t;

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

#endif // _BOARD_COMMON_USCI_I2C_H_

#ifndef _BOARD_COMMON_NATIVE_SPI_USCI_H_
#define _BOARD_COMMON_NATIVE_SPI_USCI_H_

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
    
#ifndef __MSP430_HAS_USCI_Ax__
#   error "We need USCI for SPI"
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

typedef struct spi {
    /**
     * The USCI module we are going to use
     */
    usci_t usci;
} spi_t;

/**
 * Open a connection to an SPI channel
 *
 * @param usci The USCI channel to use
 * @param clock_rate The clock rate at which we will run
 * @param out The SPI structure to fill
 *
 */
bool spi_open(usci_t usci, uint32_t clock_rate, uart_t * out);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_SPI_USCI_H_

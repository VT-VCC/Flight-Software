#ifndef _BOARD_COMMON_NATIVE_USCI_H_
#define _BOARD_COMMON_NATIVE_USCI_H_

#include <msp430.h>
#if defined (MSP430_CLASS_F5xx_6xx)
#   include "msp430f5xx_6xxgeneric.h"
#else
#   error "Unsupported processor"
#endif

#include <driverlib.h>

#ifndef __MSP430_HAS_USCI_Ax__
#   error "We need USCI for SPI"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represents which USCI module we're hooked up to
 */
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

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_USCI_H_

#ifndef _BOARD_COMMON_NATIVE_EUSCI_H_
#define _BOARD_COMMON_NATIVE_EUSCI_H_

#include <msp430.h>
#if defined (MSP430_CLASS_FR5xx_6xx)
#   include "msp430fr5xx_6xxgeneric.h"
#else
#   error "Unspported processor"
#endif

#include <driverlib.h>

#ifndef __MSP430_HAS_EUSCI_Ax__
#   error "We need EUSCI for SPI"
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

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_EUSCI_H_

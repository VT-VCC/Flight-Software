//
//  spi_eusci_native.h
//  
//
//  Created by Rohan J Dani on 10/11/17.
//
//

#ifndef spi_eusci_native_h
#define spi_eusci_native_h
#ifndef _BOARD_COMMON_NATIVE_SPI_H_
#define _BOARD_COMMON_NATIVE_SPI_H_

/// XXX: TODO: this should probably be exported to a common header
#include <msp430.h>
#if defined ( MSP430_CLASS_FR5xx_6xx )
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
    
    typedef struct spi {
        /**
         * Which EUSCI module this UART is connected to
         */
        eusci_t eusci;
    };
    
    /**
     * Open a connection to a UART channel
     *
     * @param eusci The EUSCI channel to use
     * @param baud_rate The baud rate at which we will run
     * @param out The UART structure to fill
     *
     */
    bool spi_open(eusci_t eusci, spi_t * out);
    
#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_SPI_H_

#endif /* spi_eusci_native_h */

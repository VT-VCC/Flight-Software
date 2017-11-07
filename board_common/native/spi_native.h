//
//  spi_native.h
//  
//
//  Created by Rohan J Dani on 10/11/17.
//
//

#ifndef spi_native_h
#define spi_native_h
#ifndef _BOARD_COMMON_UART_NATIVE_H_
#define _BOARD_COMMON_UART_NATIVE_H_

#ifdef __cplusplus
extern "C" {

#include <stdio.h>
    
#if defined( MSP430_CLASS_FR5xx_6xx )
#   include "spi_usci_native.h"
#   include "spi_eusci_native.h"
#else
#   error "No MSP class defined"
#endif
    
#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_SPI_NATIVE_H_

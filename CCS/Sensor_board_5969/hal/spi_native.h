#ifndef _BOARD_COMMON_NATIVE_SPI_H_
#define _BOARD_COMMON_NATIVE_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "usci_includes.h"

#if defined( USING_USCI )
#   include "spi_usci_native.h"
#elif defined( USING_EUSCI )
#   include "spi_eusci_native.h"
#else
#   error "No MSP class defined"
#endif

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_SPI_H_

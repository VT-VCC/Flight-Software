#ifndef _BOARD_COMMON_UART_NATIVE_H_
#define _BOARD_COMMON_UART_NATIVE_H_

#include "usci_includes.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined( USING_USCI )
#   include "uart_usci_native.h"
#elif defined( USING_EUSCI )
#   include "uart_eusci_native.h"
#else
#   error "No MSP class defined"
#endif

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_UART_NATIVE_H_

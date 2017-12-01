#ifndef _BOARD_COMMON_NATIVE_UART_H_
#define _BOARD_COMMON_NATIVE_UART_H_

#include "eusci_native.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uart {
    /**
     * Which EUSCI module this UART is connected to
     */
    eusci_t eusci;
} uart_t;

/**
 * Open a connection to a UART channel
 *
 * @param eusci The EUSCI channel to use
 * @param baud_rate The baud rate at which we will run
 * @param out The UART structure to fill
 *
 */
bool uart_open(eusci_t eusci, uart_baud_rate_t baud_rate, uart_t * out);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_UART_H_

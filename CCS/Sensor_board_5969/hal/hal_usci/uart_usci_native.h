#ifndef _BOARD_COMMON_USCI_UART_H_
#define _BOARD_COMMON_USCI_UART_H_

#include "usci_native.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uart {
    /**
     * The USCI module we are going to use
     */
    usci_t usci;
} uart_t;

/**
 * Open a connection to a UART channel
 *
 * @param usci The USCI channel to use
 * @param baud_rate The baud rate at which we will run
 * @param out The UART structure to fill
 *
 */
bool uart_open(usci_t usci, unsigned int baud_rate, uart_t * out);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_USCI_UART_H_

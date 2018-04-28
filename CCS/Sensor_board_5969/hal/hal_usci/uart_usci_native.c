#include "uart_usci_native.h"

#include <assert.h>

bool uart_open(usci_t on, unsigned int baud_rate, uart_t * out) {
    USCI_A_UART_initParam param;

    assert(on < USCI_count);
#ifdef USCI_B0_BASE
    assert(on < USCI_B0);
#endif

    // XXX: TODO: finish the implementation of this function
    assert(false);
}

void uart_write_byte(uart_t * out, uint8_t byte) {
    // XXX: TODO: this function
    assert(false);
}

void uart_close(uart_t * out) {
    // XXX: TODO: this function
    assert(false);
}

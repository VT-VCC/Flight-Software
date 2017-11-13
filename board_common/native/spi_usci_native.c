#include "spi_usci_native.h"

#include <assert.h>

bool spi_open(usci_t on, uint32_t clock_rate, uart_t * out) {
    //TODO
    assert(false);
}

void spi_close(spi_t * out) {
    //TODO
    assert(false);
}

spi_error_t spi_send_byte(spi_t * channel, uint8_t byte) {
    //TODO
    assert(false);
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * byte) {
    //TODO
    assert(false);
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    //TODO
    assert(false);
}

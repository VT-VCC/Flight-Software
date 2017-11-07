#include "spi_usci_native.h"

#include <assert.h>

bool spi_open(usci_t on, unsigned int baud_rate, uart_t * out) {
    USCI_A_SPI_initParam param;
    
    assert(on < USCI_count);
#ifdef USCI_B0_BASE
    assert(on < USCI_B0);
#endif
    
    // XXX: TODO: finish the implementation of this function
    assert(false);
}

void spi_write_byte(spi_t * out, uint8_t byte) {
    // XXX: TODO: this function
    assert(false);
}

void spi_close(spi_t * out) {
    // XXX: TODO: this function
    assert(false);
}

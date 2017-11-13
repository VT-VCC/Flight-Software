#include "spi.h"

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/
spi_error_t spi_send_bytes(spi_t * channel, uint8_t * send_bytes, size_t length) {
    spi_error_t err;

    for (size_t i = 0; i < length; i++) {
        err = spi_send_byte(channel, *send_bytes++);

        if (err != SPI_NO_ERROR) {
            return err;
        }
    }

    return SPI_NO_ERROR;
}

spi_error_t spi_receive_bytes(spi_t * channel, uint8_t * receive_bytes, size_t length) {
    spi_error_t err;

    for (size_t i = 0; i < length; i++) {
        err = spi_receive_byte(channel, receive_bytes++);

        if (err != SPI_NO_ERROR) {
            return err;
        }
    }

    return SPI_NO_ERROR;
}

spi_error_t spi_transfer_bytes(spi_t * channel, uint8_t * send_bytes, uint8_t * receive_bytes, size_t length) {
    spi_error_t err;

    for (size_t i = 0; i < length; i++) {
        err = spi_transfer_byte(channel, *send_bytes++, receive_bytes++);

        if (err != SPI_NO_ERROR) {
            return err;
        }
    }

    return SPI_NO_ERROR;
}

#ifndef NDEBUG
const char * spi_error_string(spi_error_t t) {
    switch(t) {
#       define STRING_OP(E) case SPI_ ## E: return #E;
        SPI_ERROR_LIST(STRING_OP)
#       undef STRING_OP
        default:
            return "SPI error unknown";
    }
}
#endif

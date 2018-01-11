#include "spi.h"

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/
spi_error_t spi_send_byte(spi_t * channel, uint8_t byte) {
    uint8_t received = 0;
    return spi_transfer_byte(channel, byte, &received);
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * byte) {
    return spi_transfer_byte(channel, 0, byte);
}

spi_error_t spi_send_bytes(spi_t * channel, uint8_t * send_bytes, size_t length) {
    return spi_transfer_bytes(channel, send_bytes, 0, length);
}

spi_error_t spi_receive_bytes(spi_t * channel, uint8_t * receive_bytes, size_t length) {
    return spi_transfer_bytes(channel, 0, receive_bytes, length);
}

spi_error_t spi_transfer_byte(spi_t * channel, uint8_t send_byte, uint8_t * receive_byte) {
    uint8_t send_bytes[1] = { send_byte };
    return spi_transfer_bytes(channel, send_bytes, receive_byte, 1);
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

#include "spi.h"

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/

void spi_open(spi_t * out) {
  
}

spi_error_t spi_send_byte(spi_t * channel, uint8_t byte) {
  //TODO! Implement using 3rdparty/driverlibs
}

spi_error_t spi_receive_byte(spi_t * channel, uint8_t * byte) {
  //TODO! Implement using 3rdparty/driverlibs
}

void spi_close(spi_t * out) {
  //TODO! Implement using 3rdparty/driverlibs
}
  
spi_error_t spi_transfer_byte(spi_t * channel,
			      uint8_t send_byte,
			      uint8_t * receive_byte) {
  //TODO! Implement using 3rdparty/driverlibs
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

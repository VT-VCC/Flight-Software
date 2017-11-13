#include "spi.h"

/******************************************************************************\
 *  Public interface implementations                                          *
\******************************************************************************/
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

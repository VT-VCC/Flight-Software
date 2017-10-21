#ifndef _BOARD_COMMON_SPI_H__
#define _BOARD_COMMON_SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//TODO! Rewrite these to be meaningful SPI errors. This is just copied over from the UART code.
/******************************************************************************\
 *  Macro list for the ways writing to a SPI can fail                        *
\******************************************************************************/
/// Macro for defining thing related to SPI errors
#define SPI_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(CHANNEL_CLOSED) \
    OP(SIGNAL_FAULT)

/// Enum representing possible error states for a SPI channel.
typedef enum spi_error {
#   define ENUM_OP(E) SPI_ ## E,
    SPI_ERROR_LIST(ENUM_OP)
#   undef ENUM_OP
    spi_count
} spi_error_t;

#ifndef NDEBUG
/// Get a string representation of the error. Only available in debug builds
const char * spi_error_string(spi_error_t t);
#endif

/** Opaque type for the SPI state
 *
 */
typedef struct spi spi_t;


/** @} */

#ifdef __cplusplus
}
#endif

#ifdef USIP_NATIVE
#   include "spi_native.h"
#else
#   include "spi_test.hpp"
#endif

#endif // _BOARD_COMMON_SPI_H_

#ifndef _BOARD_COMMON_SPI_H__
#define _BOARD_COMMON_SPI_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *  Macro list for the ways writing to a SPI can fail                        *
\******************************************************************************/
/// Macro for defining thing related to SPI errors
#define SPI_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(CHANNEL_CLOSED)

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

/** Open the given SPI channel so that it can be used.
 * @param The SPI channel to open.
 */
void spi_open(spi_t * out);

/** Safely close the SPI channel so that it can be reused later.
 * @param The SPI channel to close.
 */
void spi_close(spi_t * out);

/** Transfer a byte by SPI.
 * @param channel The SPI channel to send to.
 * @param byte The byte to send.
 * @return An error code. This should always be checked.
 */
spi_error_t spi_send_byte(spi_t * channel,
			  uint8_t byte);

/** Receive a byte from the SPI buffer.
 * @param channel The SPI channel to read from.
 * @param byte The address to write the byte to.
 * @return An error code. This should always be checked.
 */
spi_error_t spi_receive_byte(spi_t * channel,
			     uint8_t * byte);
  
/** Transfer a byte from SPI and save the return byte.
 * @param channel The channel to read from.
 * @param send_byte The byte to send.
 * @param receive_byte The address of the byte to save to.
 *
 * @return An error code. This should always be checked.
 */
spi_error_t spi_transfer_byte(spi_t * channel,
			      uint8_t send_byte,
			      uint8_t * receive_byte);

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

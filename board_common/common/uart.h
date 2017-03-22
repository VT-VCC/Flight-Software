#ifndef _BOARD_COMMON_UART_H_
#define _BOARD_COMMON_UART_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *  Macro list for the ways writing to a UART can fail                        *
\******************************************************************************/
/// Macro for defining thing related to UART errors
#define UART_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(CHANNEL_CLOSED) \
    OP(SIGNAL_FAULT)

/// Enum representing possible error states for a UART channel.
typedef enum uart_error {
#   define ENUM_OP(E) UART_ ## E,
    UART_ERROR_LIST(ENUM_OP)
#   undef ENUM_OP
    UART_count
} uart_error_t;

#ifndef NDEBUG
/// Get a string representation of the error. Only available in debug builds
const char * uart_error_string(uart_error_t t);
#endif


/******************************************************************************\
 *  UART type                                                                 *
\******************************************************************************/

/** @defgroup uart_native Native UART components
 *  These are the components of the UART system that are target-dependent. That
 *  is, their implementation depends on if we are building in test or target
 *  mode.
 *  @{
 */

/** Opaque type for the UART state
 *
 */
typedef struct uart uart_t;

/**
 * Open a connection to the UART channel
 *
 * @TODO Revisit this API. We probably need more parameters.
 *
 * @param out The output parameter
 * @param baud_rate The baud rate at which we should be running
 *
 * @return True if the UART channel opens successfully
 */
bool uart_open(uart_t * out, size_t baud_rate);

/** Write a byte to the UART channel.
 *
 * Possible return values:
 *  \li \verbatim UART_NO_ERROR \endverbatim when no error occurs.
 *  \li \verbatim UART_CHANEL_CLOSED \endverbatim if the channel is not open
 *  \li \verbatim UART_SIGNAL_FAULT \endverbatim if the UART implementation
 *   has detected a signal integrity error.
 *
 * @param channel The UART device to write to
 * @param byte The data byte to write
 *
 * @return UART error enumeration representing the error, see docs.
 */
uart_error_t uart_write_byte(uart_t * channel, uint8_t byte);

/** Block read of a byte from the UART channel
 *
 * Possible return values:
 *  \li \verbatim UART_NO_ERROR \endverbatim when no error occurs.
 *  \li \verbatim UART_CHANEL_CLOSED \endverbatim if the channel is not open
 *  \li \verbatim UART_SIGNAL_FAULT \endverbatim if the UART implementation
 *   has detected a signal integrity error.
 *
 * @param channel The channel to read from
 * @param output Memory location of a byte to read
 *
 * @return UART error enumeration representing the error, see docs.
 */
uart_error_t uart_read_byte(uart_t * channel, uint8_t * output);

/** @} */

/** @defgroup uart_common Common components
 *  These are the components of the UART system that are target-independent.
 *  That is, their implementation is shared between all targets
 *  @{
 */

/** Write a full buffer to the UART channel
 *
 * Possible return values:
 *  \li \verbatim UART_NO_ERROR \endverbatim when no error occurs.
 *  \li \verbatim UART_CHANEL_CLOSED \endverbatim if the channel is not open
 *  \li \verbatim UART_SIGNAL_FAULT \endverbatim if the UART implementation
 *   has detected a signal integrity error.
 *
 * @param channel The channel to write to
 * @param bytes Pointer to the buffer which will be written out
 * @param n The number of bytes to write
 *
 * @return UART error enumeration representing the error, see docs.
 */
uart_error_t uart_write_bytes(uart_t * channel, uint8_t * bytes, size_t n);

/** Blocking read of several bytes from a UART channel
 *
 * Possible return values:
 *  \li \verbatim UART_NO_ERROR \endverbatim when no error occurs.
 *  \li \verbatim UART_CHANEL_CLOSED \endverbatim if the channel is not open
 *  \li \verbatim UART_SIGNAL_FAULT \endverbatim if the UART implementation
 *   has detected a signal integrity error.
 *
 * @param channel The channel to read from
 * @param bytes Pointer to a buffer in to which we should read
 * @param n The number of bytes to read
 *
 * @return UART error enumeration representing the error, see docs.
 */
uart_error_t uart_read_bytes(uart_t * channel, uint8_t * bytes, size_t n);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_UART_H_

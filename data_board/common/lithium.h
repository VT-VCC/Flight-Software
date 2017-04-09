#ifndef _COMMON_LITHIUM_H_
#define _COMMON_LITHIUM_H_

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Macro list for results of operations on the Lithium
 */
#define LITHIUM_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(BAD_COMMUNICATION)

/**
 * Enumeration of possible results for trying to communicate with the Lithium
 */
typedef enum lithium_result {
#   define STRING_OP(E) LITHIUM_ ## E,
    LITHIUM_RESULT_LIST(STRING_OP)
#   undef STRING_OP
    LITHIUM_count
} lithium_result_t;


/**
 * Represents a connection to a Lithium radio
 */
typedef struct lithium {
    /**
     * The UART channel over which we talk
     */
    uart_t uart;
} lithium_t;

/**
 * Open a connection to a Lithium radio.
 *
 * @param out Output parameter
 *
 * @return True if and only if the radio connection succeeds
 */
bool lithium_open(lithium_t * out);

/**
 * Close a connection to a lithium radio.
 *
 * @param radio The radio to close
 */
void lithium_close(lithium_t * out);

/**
 * Send a noop command to an open lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return LITHIUM_OK if and only if communication succeeds.
 */
lithium_result_t lithium_send_noop(lithium_t * radio);

/*
	* Send a transmit command to open lithium radio
	*
	* @param radio The radio to communicate with
	*
	* @return LITHIUM_OK if and only if communication succeeds.
	*

 */
lithium_result_t lithium_send_transmit(lithium_t * radio, lithium_t * push, uint16_t size);

/*
* Send a transmit receive command to open lithium radio
*
* @param radio The radio to communicate with
*
* @return LITHIUM_OK if and only if communication succeeds.
*

*/

lithium_result_t lithium_receive(lithium_t * radio, lithium_t * pull);

#ifdef __cplusplus
}
#endif

#endif // _COMMON_LITHIUM_H_

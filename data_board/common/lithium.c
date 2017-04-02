#include "lithium.h"

#include "lithium_internal.h"

/******************************************************************************\
 *  Private support functions                                                 *
\******************************************************************************/
void compute_checksum(uint8_t * data, size_t len, uint8_t * output);
lithium_result_t lithium_send_header(lithium_t * radio, uint8_t command, uint16_t size);

/******************************************************************************\
 *  Pulic interface implementations                                           *
\******************************************************************************/
bool lithium_open(lithium_t * out) {
    /// XXX confirm that this is the correct baud rate
    return uart_open(&(out->uart), 9600);
}

void lithium_close(lithium_t * out) {
    uart_close(&out->uart);
}

lithium_result_t lithium_send_noop(lithium_t * radio) {
    return lithium_send_header(radio, NO_OP_COMMAND, 0);
}

// Haroun 3/31/2017

lithium_result_t lithium_send_transmit(lithium_t * radio, lithium_t * push, uint16_t size ) {
    
	if(lithium_send_header(radio, TRANSMIT_DATA, payload_size) != LITHIUM_BAD_COMMUNICATION)
	{
		return lithium_packet_send(radio, push, payload_size);
	}
	
	return LITHIUM_BAD_COMMUNICATION;
	
	
}

lithium_result_t lithium_receive(lithium_t * radio, lithium_t * pull){
	return lithium_packet_receive(radio, pull);	
	
}

// End Haroun 3/31/2017

/******************************************************************************\
 *  Private support function implementations                                  *
\******************************************************************************/
void compute_checksum(uint8_t * data, size_t len, uint8_t * output) {
    uint8_t a = 0;
    uint8_t b = 0;
    for (size_t i = 0; i < len; ++i) {
        a += data[i];
        b += a;
    }
    output[0] = a;
    output[1] = b;
}

lithium_result_t lithium_send_header(lithium_t * radio, uint8_t command, uint16_t size) {
    uint8_t header[8];
    header[0] = 0x48; // 'H'
    header[1] = 0x65; // 'E'
    header[2] = 0x10;
    header[3] = command;
    header[4] = (size >> 8) & 0xFF;
    header[5] = size & 0xFF;
    compute_checksum(header, 6, header + 6);

    uart_error_t err = uart_write_bytes(&radio->uart, header, 8);
    if (err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }
    return LITHIUM_NO_ERROR;
}

lithium_result_t lithium_packet_send(lithium_t * radio, lithium_t * push, uint16_t size){
	uart_error_t err = uart_write_bytes(&radio->uart, &push, size);
    if (err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }
    return LITHIUM_NO_ERROR;	
	
}

lithium_result_t lithium_packet_receive(lithium_t * radio, lithium_t * pull){
	uart_error_t err = uart_read_bytes(&radio->uart, &pull, 8);
    if (err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }
    return LITHIUM_NO_ERROR;	
	
}








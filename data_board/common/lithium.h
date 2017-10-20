#ifndef _COMMON_LITHIUM_H_
#define _COMMON_LITHIUM_H_

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

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
 * Macro list for results of operations on the Lithium
 */
#define LITHIUM_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(BAD_COMMUNICATION) \
    OP(INVALID_PACKET)

/**
 * Enumeration of possible results for trying to communicate with the Lithium
 */
typedef enum lithium_result {
#   define STRING_OP(E) LITHIUM_ ## E,
    LITHIUM_RESULT_LIST(STRING_OP)
#   undef STRING_OP
    LITHIUM_count
} lithium_result_t;

#undef LITHIUM_RESULT_LIST


#define LITHIUM_COMMAND_TYPE_LIST(TYPE) \
    TYPE(I_MESSAGE, 0x10) \
    TYPE(O_MESSAGE, 0x20)

typedef enum lithium_command_type {
#   define STRING_OP(E, V) LITHIUM_##E = V,
    LITHIUM_COMMAND_TYPE_LIST(STRING_OP)
#   undef STRING_OP
} lithium_command_type_t;

#undef LITHIUM_COMMAND_TYPE_LIST


#define LITHIUM_COMMAND_LIST(COMMAND) \
    COMMAND(NO_OP, 0x01) \
    COMMAND(RESET_SYSTEM, 0x02) \
    COMMAND(TRANSMIT_DATA, 0x03) \
    COMMAND(RECEIVE_DATA, 0x04) \
    COMMAND(GET_TRANSCEIVER_CONFIG, 0x05) \
    COMMAND(SET_TRANSCEIVER_CONFIG, 0x06) \
    COMMAND(TELEMETRY_QUERY, 0x07) \
    COMMAND(WRITE_FLASH, 0x08) \
    COMMAND(RF_CONFIG, 0x09) \
    COMMAND(BEACON_DATA, 0x10) \
    COMMAND(BEACON_CONFIG, 0x11) \
    COMMAND(READ_FIRMWARE_REVISION, 0x12) \
    COMMAND(WRITE_OVER_AIR_KEY, 0x13) \
    COMMAND(FIRMWARE_UPDATE, 0x14) \
    COMMAND(FIRMWARE_PACKET, 0x15) \
    COMMAND(FAST_PA_SET, 0x20)

typedef enum lithium_command {
#   define STRING_OP(E, V) LITHIUM_COMMAND_##E = V,
    LITHIUM_COMMAND_LIST(STRING_OP)
#   undef STRING_OP
    LITHIUM_COMMAND_count
} lithium_command_t;

#undef LITHIUM_COMMAND_LIST


typedef struct lithium_packet {
    lithium_command_type_t type;
    lithium_command_t command;
    uint16_t payload_length;
    uint8_t payload[255];
} lithium_packet_t;


typedef struct lithium_config {
    uint8_t interface_baud_rate;   //Radio Interface Baud Rate (9600=0x00)
    uint8_t tx_power_amp_level;    //Tx Power Amp level (min = 0x00 max = 0xFF)
    uint8_t rx_rf_baud_rate;       //Radio RX RF Baud Rate (9600=0x00)
    uint8_t tx_rf_baud_rate;       //Radio TX RF Baud Rate (9600=0x00)
    uint8_t rx_modulation;         //(0x00 = GFSK);
    uint8_t tx_modulation;         //(0x00 = GFSK);
    uint32_t rx_freq;               //Channel Rx Frequency (ex: 45000000)
    uint32_t tx_freq;               //Channel Tx Frequency (ex: 45000000)
    unsigned char source[6];      //AX25 Mode Source Call Sign (default NOCALL)
    unsigned char destination[6]; //AX25 Mode Destination Call Sign (default CQ)
    uint16_t tx_preamble;           //AX25 Mode Tx Preamble Byte Length (0x00 = 20 flags)
    uint16_t tx_postamble;          //AX25 Mode Tx Postamble Byte Length (0x00 = 20 flags)
    uint16_t function_config;       //Radio Configuration Discrete Behaviors
    uint16_t function_config2;    //Radio Configuration Discrete Behaviors #2
} lithium_config_t;

typedef struct lithium_rf_config {
    uint8_t front_end_level; //0 to 63 Value
    uint8_t tx_power_amp_level; //0 to 255 value, non-linear
    uint32_t tx_frequency_offset; //Up to 20 kHz
    uint32_t rx_frequency_offset; //Up to 20 kHz
} lithium_rf_config_t;

typedef struct lithium_beacon_config {
    uint8_t beacon_interval;  //value of 0 is off, 2.5 sec delay per LSB
} lithium_beacon_config_t;

typedef struct lithium_telem {
    uint16_t op_counter;
    int16_t msp430_temp;
    uint8_t time_count[3];
    uint8_t rssi;
    uint32_t bytes_received;
    uint32_t bytes_transmitted;
} lithium_telem_t;


/**
 * Open a connection to a Lithium radio.
 *
 * @param out Output parameter
 * @param uart The UART channel to use. Takes ownership of the lifecycle of this
 *      channel
 *
 * @return True if and only if the radio connection succeeds
 */
bool lithium_open(lithium_t * out, uart_t * uart);

/**
 * Close a connection to a lithium radio.
 *
 * @param radio The radio to close
 */
void lithium_close(lithium_t * out);

lithium_result_t lithium_send_packet(lithium_t * radio, lithium_packet_t * packet);

lithium_result_t lithium_send_no_op(lithium_t * radio);

lithium_result_t lithium_send_reset(lithium_t * radio);

lithium_result_t lithium_send_transmit(lithium_t * radio, uint8_t * data, uint16_t length);

lithium_result_t lithium_send_get_config(lithium_t * radio);

lithium_result_t lithium_send_set_config(lithium_t * radio, lithium_config_t * config);

lithium_result_t lithium_send_query_telem(lithium_t * radio);

lithium_result_t lithium_send_write_flash(lithium_t * radio, uint8_t * hash);

lithium_result_t lithium_send_set_rf_config(lithium_t * radio, lithium_rf_config_t * config);

lithium_result_t lithium_send_set_beacon(lithium_t * radio, uint8_t * data, uint16_t length);

lithium_result_t lithium_send_set_beacon_config(lithium_t * radio, lithium_beacon_config_t * config);

lithium_result_t lithium_send_read_fw_version(lithium_t * radio);

lithium_result_t lithium_send_write_dio_key(lithium_t * radio, uint8_t * key);

lithium_result_t lithium_send_begin_fw_update(lithium_t * radio, uint8_t * hash);

lithium_result_t lithium_send_stream_fw_update(lithium_t * radio, uint8_t * data, uint16_t length);

lithium_result_t lithium_send_set_pa_level(lithium_t * radio, uint8_t speed);

lithium_result_t lithium_parse_packet(uint8_t * data, uint16_t length, lithium_packet_t * packet);

#ifdef __cplusplus
}
#endif

#endif // _COMMON_LITHIUM_H_

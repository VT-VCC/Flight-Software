#ifndef _COMMON_LITHIUM_H_
#define _COMMON_LITHIUM_H_

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The connection to a Lithium radio
 */
typedef struct lithium {
    /**
     * The owned UART channel over which we talk
     */
    uart_t uart;
} lithium_t;


/**
 * Macro list for results of operations on the Lithium
 */
#define LITHIUM_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(BAD_COMMUNICATION) \
    OP(INVALID_PACKET) \
    OP(INVALID_CHECKSUM)

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


/**
 * Macro list for the types of a Lithium packet
 */
#define LITHIUM_COMMAND_TYPE_LIST(TYPE) \
    TYPE(I_MESSAGE, 0x10) \
    TYPE(O_MESSAGE, 0x20)

/**
 * Enumeration of the types of Lithium packets
 */
typedef enum lithium_command_type {
#   define STRING_OP(E, V) LITHIUM_##E = V,
    LITHIUM_COMMAND_TYPE_LIST(STRING_OP)
#   undef STRING_OP
} lithium_command_type_t;

#undef LITHIUM_COMMAND_TYPE_LIST

/**
 * Macro list for the types of Lithium commands
 */
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

/**
 * Enumeration for the types of Lithium commands
 */
typedef enum lithium_command {
#   define STRING_OP(E, V) LITHIUM_COMMAND_##E = V,
    LITHIUM_COMMAND_LIST(STRING_OP)
#   undef STRING_OP
    LITHIUM_COMMAND_count
} lithium_command_t;

#undef LITHIUM_COMMAND_LIST

/**
 * Macro list for UART baud rates
 */
#define LITHIUM_BAUD_LIST(BAUD) \
    BAUD(9600, 0) \
    BAUD(19200, 1) \
    BAUD(38400, 2) \
    BAUD(76800, 3) \
    BAUD(115200, 4)

/**
 * Enumeration for the UART baud rates used in the front-end configuration
 */
typedef enum lithium_baud {
#   define STRING_OP(E, V) LITHIUM_BAUD_##E = V,
    LITHIUM_BAUD_LIST(STRING_OP)
#   undef STRING_OP
} lithium_baud_t;

#undef LITHIUM_BAUD_LIST

/**
 * Macro list for the RF baud rates
 */
#define LITHIUM_RF_BAUD_LIST(BAUD) \
    BAUD(1200, 0) \
    BAUD(9600, 1) \
    BAUD(19200, 2) \
    BAUD(38400, 3)

/**
 * Enumeration for the RF baud rates used in the RF configuration
 */
typedef enum lithium_rf_baud {
#   define STRING_OP(E, V) LITHIUM_RF_BAUD_##E = V,
    LITHIUM_RF_BAUD_LIST(STRING_OP)
#   undef STRING_OP
} lithium_rf_baud_t;

#undef LITHIUM_RF_BAUD_LIST

/**
 * Macro list for the RF modulation methods
 */
#define LITHIUM_RF_MOD_LIST(MOD) \
    MOD(GFSK, 0) \
    MOD(AFSK, 1) \
    MOD(BPSK, 2)

/**
 * Enumeration for the RF modulation methods used in the RF configuration
 */
typedef enum lithium_rf_mod {
#   define STRING_OP(E, V) LITHIUM_RF_MOD_##E = V,
    LITHIUM_RF_MOD_LIST(STRING_OP)
#   undef STRING_OP
} lithium_rf_mod_t;

#undef LITHIUM_RF_MOD_LIST

/**
 * The packet representation used to communicate with a Lithium radio
 */
typedef struct lithium_packet {
    /**
     * The type of the packet, either to (I-Message) or from (O-Message) the
     * radio
     */
    lithium_command_type_t type;
    /**
     * The command of the packet
     */
    lithium_command_t command;
    /**
     * The length of the payload, also used to indicate ACK/NACK in O-Messages
     */
    uint16_t payload_length;
    /**
     * The packet's payload
     */
    uint8_t payload[255];
} lithium_packet_t;


/**
 * The front-end configuration of a Lithium radio
 */
typedef struct lithium_config {
    lithium_baud_t interface_baud_rate;   //Radio Interface Baud Rate (9600=0x00)
    uint8_t tx_power_amp_level;    //Tx Power Amp level (min = 0x00 max = 0xFF)
    lithium_rf_baud_t rx_rf_baud_rate;       //Radio RX RF Baud Rate (9600=0x00)
    lithium_rf_baud_t tx_rf_baud_rate;       //Radio TX RF Baud Rate (9600=0x00)
    lithium_rf_mod_t rx_modulation;         //(0x00 = GFSK);
    lithium_rf_mod_t tx_modulation;         //(0x00 = GFSK);
    uint32_t rx_freq;               //Channel Rx Frequency (ex: 45000000)
    uint32_t tx_freq;               //Channel Tx Frequency (ex: 45000000)
    unsigned char source[6];      //AX25 Mode Source Call Sign (default NOCALL)
    unsigned char destination[6]; //AX25 Mode Destination Call Sign (default CQ)
    uint16_t tx_preamble;           //AX25 Mode Tx Preamble Byte Length (0x00 = 20 flags)
    uint16_t tx_postamble;          //AX25 Mode Tx Postamble Byte Length (0x00 = 20 flags)
    uint16_t function_config;       //Radio Configuration Discrete Behaviors
    uint16_t function_config2;    //Radio Configuration Discrete Behaviors #2
} lithium_config_t;

/**
 * The configuration of a Lithium's RF module
 */
typedef struct lithium_rf_config {
    uint8_t front_end_level; //0 to 63 Value
    uint8_t tx_power_amp_level; //0 to 255 value, non-linear
    uint32_t tx_frequency_offset; //Up to 20 kHz
    uint32_t rx_frequency_offset; //Up to 20 kHz
} lithium_rf_config_t;

/**
 * The configuration of a Lithium's beacon system
 */
typedef struct lithium_beacon_config {
    uint8_t beacon_interval;  //value of 0 is off, 2.5 sec delay per LSB
} lithium_beacon_config_t;

/**
 * The telemetry packet describing the radio's status
 */
typedef struct lithium_telem {
    uint16_t op_counter;
    int16_t msp430_temp;
    uint8_t time_count[3];
    uint8_t rssi;
    uint32_t bytes_received;
    uint32_t bytes_transmitted;
} lithium_telem_t;


/**
 * Open a connection to a Lithium radio
 *
 * @param radio The output radio object
 * @param uart The UART channel to use. Takes ownership of the lifecycle of this
 *      channel
 *
 * @return True if and only if the radio connection succeeds
 */
bool lithium_open(lithium_t * radio, uart_t * uart);

/**
 * Close a connection to a Lithium radio
 *
 * @param radio The radio to close
 */
void lithium_close(lithium_t * radio);

/**
 * Send a packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param packet The packet to send
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_packet(lithium_t * radio, lithium_packet_t * packet);

/**
 * Send a no-operation packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_no_op(lithium_t * radio);

/**
 * Send a system reset packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_reset(lithium_t * radio);

/**
 * Send a data transmission packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param data The transmission payload to send
 * @param length The length of the transmission payload
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_transmit(lithium_t * radio, uint8_t * data, uint16_t length);

/**
 * Send a radio configuration request packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_get_config(lithium_t * radio);

/**
 * Send a radio configuration change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param config The configuration to set the radio to
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_set_config(lithium_t * radio, lithium_config_t * config);

/**
 * Send a telemetry query packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_query_telem(lithium_t * radio);

/**
 * Send a radio configuration write-to-flash packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param hash The 16 byte MD5 hash of the radio configuration
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_write_flash(lithium_t * radio, uint8_t * hash);

/**
 * Send an RF configuration change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param config The configuration to send to the RF module
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_set_rf_config(lithium_t * radio, lithium_rf_config_t * config);

/**
 * Send a beacon data change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param data The contents of the beacon
 * @param length The length of the beacon contents
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_set_beacon(lithium_t * radio, uint8_t * data, uint16_t length);

/**
 * Send a beacon configuration change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param config The configuration to send to the beacon module
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_set_beacon_config(lithium_t * radio, lithium_beacon_config_t * config);

/**
 * Send a firmware version request packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_read_fw_version(lithium_t * radio);

/**
 * Send an over-the-air key change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param key The 4 byte key to change to
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_write_dio_key(lithium_t * radio, uint8_t * key);

/**
 * Send a firmware update initiation packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param hash The 16 byte MD5 hash of the firmware update
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_begin_fw_update(lithium_t * radio, uint8_t * hash);

/**
 * Send a firmware update chunk packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param data The chunk of the firmware update
 * @param length The length of the chunk
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_stream_fw_update(lithium_t * radio, uint8_t * data, uint16_t length);

/**
 * Send a power amplifier level change packet to a Lithium radio
 *
 * @param radio The radio to communicate with
 * @param speed The speed to change the power amplifier level to
 *
 * @return The result of the operation
 */
lithium_result_t lithium_send_set_pa_level(lithium_t * radio, uint8_t speed);

/**
 * Attempt to receive and parse a Lithium packet over the UART channel
 *
 * @param radio The radio to communicate with
 * @param packet The output packet parsed from the channel
 *
 * @return The result of the operation
 */
lithium_result_t lithium_receive_packet(lithium_t * radio, lithium_packet_t * packet);

/**
 * Parse the header of an encoded Lithium packet
 *
 * @param raw_packet The encoded packet header to parse
 * @param packet The output packet with header fields populated
 * @param remaining_bytes The output number of bytes to read to parse a body
 *
 * @return The result of the operation
 */
lithium_result_t lithium_parse_header(uint8_t * raw_packet, lithium_packet_t * packet, uint16_t * remaining_bytes);

/**
 * Parse the body of an encoded Lithium packet
 *
 * @param raw_packet The entire encoded packet to parse
 * @param packet The completed output packet
 *
 * @return The result of the operation
 */
lithium_result_t lithium_parse_body(uint8_t * raw_packet, lithium_packet_t * packet);

/**
 * Check if a packet is an I-Message
 *
 * @param packet The packet to inspect
 *
 * @return True if packet is an I-Message
 */
bool lithium_is_i_message(lithium_packet_t * packet);

/**
 * Check if a packet is an O-Message
 *
 * @param packet The packet to inspect
 *
 * @return True if packet is an O-Message
 */
bool lithium_is_o_message(lithium_packet_t * packet);

/**
 * Check if a packet is indicating an acknowledgement (ACK)
 *
 * @param packet The packet to inspect
 *
 * @return True if packet is an I-Message
 */
bool lithium_is_ack(lithium_packet_t * packet);

/**
 * Check if a packet is indicating a non-acklowedgement (NACK)
 *
 * @param packet The packet to inspect
 *
 * @return True if packet has the size of a NACK packet
 */
bool lithium_is_nack(lithium_packet_t * packet);


#ifdef __cplusplus
}
#endif

#endif // _COMMON_LITHIUM_H_

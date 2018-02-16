#include <string.h>
#include "lithium.h"

#include "lithium_internal.h"

/******************************************************************************\
 *  Private support functions                                                 *
\******************************************************************************/
void compute_checksum(uint8_t * data, size_t length, uint8_t * output);
uint16_t encode_header(lithium_command_type_t type, lithium_command_t command, uint16_t payload_length, uint8_t * raw_packet);
uint16_t encode_body(void * payload, uint16_t length, uint8_t * raw_packet);
lithium_result_t send_data(lithium_t * radio, uint8_t * raw_packet, uint16_t length);

/******************************************************************************\
 *  Pulic interface implementations                                           *
\******************************************************************************/

bool lithium_open(lithium_t * radio, uart_t * uart) {
    radio->uart = *uart;
    return true;
}

void lithium_close(lithium_t * radio) {
    uart_close(&radio->uart);
}


lithium_result_t lithium_send_packet(lithium_t * radio, lithium_packet_t * packet) {
    // Encode the header and payload into a buffer
    uint8_t raw_packet[MAX_PACKET_LENGTH];
    uint16_t packet_length = encode_header(packet->type, packet->command, packet->payload_length, raw_packet);
    if (packet->payload_length > 0) {
        packet_length += encode_body(packet->payload, packet->payload_length, raw_packet);
    }

    // Send the buffer
    return send_data(radio, raw_packet, packet_length);
}


// Generate methods to send header-only packets
#define EMIT_SEND(name, command) \
    lithium_result_t lithium_send_##name(lithium_t * radio) { \
        uint8_t raw_packet[HEADER_LENGTH]; \
        uint16_t packet_length = encode_header(LITHIUM_I_MESSAGE, LITHIUM_COMMAND_##command, 0, raw_packet); \
        return send_data(radio, raw_packet, packet_length); \
    }

EMIT_SEND(no_op, NO_OP)
EMIT_SEND(reset, RESET_SYSTEM)
EMIT_SEND(get_config, GET_TRANSCEIVER_CONFIG)
EMIT_SEND(query_telem, TELEMETRY_QUERY)
EMIT_SEND(read_fw_version, READ_FIRMWARE_REVISION)

#undef EMIT_SEND


// Generate methods to send packets with payloads
#define EMIT_SEND_PAYLOAD(name, command, data_param, length_param, ...) \
    lithium_result_t lithium_send_##name(lithium_t * radio, __VA_ARGS__) {\
        uint8_t raw_packet[MAX_PACKET_LENGTH]; \
        uint16_t packet_length = encode_header(LITHIUM_I_MESSAGE, LITHIUM_COMMAND_##command, length_param, raw_packet); \
        packet_length +=  encode_body(data_param, length_param, raw_packet); \
        return send_data(radio, raw_packet, packet_length); \
    }

EMIT_SEND_PAYLOAD(transmit, TRANSMIT_DATA,
    data, length,
    uint8_t * data, uint16_t length)
EMIT_SEND_PAYLOAD(set_config, SET_TRANSCEIVER_CONFIG,
    config, sizeof(lithium_config_t),
    lithium_config_t * config)
EMIT_SEND_PAYLOAD(write_flash, WRITE_FLASH,
    hash, 16,
    uint8_t * hash)
EMIT_SEND_PAYLOAD(set_rf_config, RF_CONFIG,
    config, sizeof(lithium_rf_config_t),
    lithium_rf_config_t * config)
EMIT_SEND_PAYLOAD(set_beacon, BEACON_DATA,
    data, length,
    uint8_t * data, uint16_t length)
EMIT_SEND_PAYLOAD(set_beacon_config, BEACON_CONFIG,
    config, sizeof(lithium_beacon_config_t),
    lithium_beacon_config_t * config)
EMIT_SEND_PAYLOAD(write_dio_key, WRITE_OVER_AIR_KEY,
    key, 16,
    uint8_t * key)
EMIT_SEND_PAYLOAD(begin_fw_update, FIRMWARE_UPDATE,
    hash, 16,
    uint8_t * hash)
EMIT_SEND_PAYLOAD(stream_fw_update, FIRMWARE_PACKET,
    data, length,
    uint8_t * data, uint16_t length)
EMIT_SEND_PAYLOAD(set_pa_level, FAST_PA_SET,
    &output_level, 1,
    uint8_t output_level)

#undef EMIT_SEND_PAYLOAD


lithium_result_t lithium_receive_packet(lithium_t * radio, lithium_packet_t * packet) {
    // Try to read a header
    const uint16_t raw_packet_length = 255;//MAX_PACKET_LENGTH;
    uint8_t raw_packet[255];
    uart_error_t uart_err = uart_read_bytes(&radio->uart, raw_packet, HEADER_LENGTH);
    if (uart_err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }

    // Parse the header and see if there's a payload
    uint16_t remaining_bytes;
    lithium_result_t err = lithium_parse_header(raw_packet, raw_packet_length, packet, &remaining_bytes);
    if (err != LITHIUM_NO_ERROR) {
        return err;
    }

    // Try to read a payload
    if (remaining_bytes > 0) {
        uart_err = uart_read_bytes(&radio->uart, raw_packet + HEADER_LENGTH, remaining_bytes);
        if (uart_err != UART_NO_ERROR) {
            return LITHIUM_BAD_COMMUNICATION;
        }

        err = lithium_parse_body(raw_packet, raw_packet_length, packet);
        if (err != LITHIUM_NO_ERROR) {
            return err;
        }
    }

    return LITHIUM_NO_ERROR;
}


lithium_result_t lithium_parse_header(uint8_t * raw_packet, uint16_t raw_packet_length, lithium_packet_t * packet, uint16_t * remaining_bytes) {
    if (raw_packet_length < HEADER_LENGTH) {
        return LITHIUM_INVALID_PACKET;
    }

    // Validate magic bytes
    bool has_sync_bytes = (raw_packet[0] == SYNC_1 && raw_packet[1] == SYNC_2);
    if (!has_sync_bytes) {
        return LITHIUM_INVALID_PACKET;
    }

    lithium_command_type_t type = raw_packet[2];
    lithium_command_t command = raw_packet[3];

    // Validate command
    bool is_valid_message_type = (type == LITHIUM_I_MESSAGE || type == LITHIUM_O_MESSAGE);
    bool is_valid_command = (command < LITHIUM_COMMAND_count);
    if (!is_valid_message_type || !is_valid_command) {
        return LITHIUM_INVALID_PACKET;
    }

    uint16_t payload_length = (raw_packet[4] << 8) | raw_packet[5];

    // Validate payload length
    bool is_ack_nack_packet = (
        type == LITHIUM_O_MESSAGE &&
        (payload_length == ACK_LENGTH || payload_length == NACK_LENGTH));
    bool has_valid_payload_size = (payload_length <= MAX_PAYLOAD_LENGTH);
    if (!is_ack_nack_packet && !has_valid_payload_size) {
        return LITHIUM_INVALID_PACKET;
    }

    uint8_t header_checksum[CHECKSUM_LENGTH];
    compute_checksum(raw_packet + SYNC_BYTES_LENGTH, HEADER_DATA_LENGTH, header_checksum);

    // Validate header checksums
    bool header_checksums_match = (
        raw_packet[6] == header_checksum[0] &&
        raw_packet[7] == header_checksum[1]);
    if (!header_checksums_match) {
        return LITHIUM_INVALID_CHECKSUM;
    }

    // Indicate the number of bytes to read for a body
    if (!is_ack_nack_packet && payload_length > 0) {
        *remaining_bytes = payload_length + CHECKSUM_LENGTH;
    }
    else {
        *remaining_bytes = 0;
    }

    // Populate remaining fields
    packet->type = type;
    packet->command = command;
    packet->payload_length = payload_length;

    return LITHIUM_NO_ERROR;
}

lithium_result_t lithium_parse_body(uint8_t * raw_packet, uint16_t raw_packet_length, lithium_packet_t * packet) {
    if (raw_packet_length < HEADER_LENGTH) {
        return LITHIUM_INVALID_PACKET;
    }

    uint16_t payload_length = packet->payload_length;
    uint8_t payload_checksum[CHECKSUM_LENGTH];
    compute_checksum(raw_packet + SYNC_BYTES_LENGTH, HEADER_DATA_LENGTH + CHECKSUM_LENGTH + payload_length, payload_checksum);

    // Validate payload checksums
    bool payload_checksums_match = (
        raw_packet[HEADER_LENGTH+payload_length] == payload_checksum[0] &&
        raw_packet[HEADER_LENGTH+payload_length+1] == payload_checksum[1]);
    if (!payload_checksums_match) {
        return LITHIUM_INVALID_CHECKSUM;
    }

    if (raw_packet_length < HEADER_LENGTH + payload_length + CHECKSUM_LENGTH) {
        return LITHIUM_INVALID_PACKET;
    }

    // Copy payload
    memcpy(packet->payload, raw_packet + HEADER_LENGTH, payload_length);

    return LITHIUM_NO_ERROR;
}


bool lithium_is_i_message(lithium_packet_t * packet) {
    return packet->type == LITHIUM_I_MESSAGE;
}

bool lithium_is_o_message(lithium_packet_t * packet) {
    return packet->type == LITHIUM_O_MESSAGE;
}

bool lithium_is_ack(lithium_packet_t * packet) {
    return packet->type == LITHIUM_O_MESSAGE && packet->payload_length == ACK_LENGTH;
}

bool lithium_is_nack(lithium_packet_t * packet) {
    return packet->type == LITHIUM_O_MESSAGE && packet->payload_length == NACK_LENGTH;
}

/******************************************************************************\
 *  Private support function implementations                                  *
\******************************************************************************/
/**
 * Calculate the Fletcher-16 checksum of binary data
 *
 * @param data The data to calculate the checksum of
 * @param length The length of the data
 * @param output The 2-byte checksum output
 */
void compute_checksum(uint8_t * data, size_t length, uint8_t * output) {
    uint8_t a = 0;
    uint8_t b = 0;
    for (size_t i = 0; i < length; ++i) {
        a += data[i];
        b += a;
    }
    output[0] = a;
    output[1] = b;
}

/**
 * Encode a Lithium packet's header
 *
 * @param type The type of the packet
 * @param command The packet's command
 * @param payload_length The length of the packet's payload
 * @param raw_packet The output byte array of at least 8 bytes
 *
 * @return The size of the packet header in bytes
 */
uint16_t encode_header(lithium_command_type_t type, lithium_command_t command, uint16_t payload_length, uint8_t * raw_packet) {
    raw_packet[0] = SYNC_1;
    raw_packet[1] = SYNC_2;
    raw_packet[2] = type;
    raw_packet[3] = command;
    raw_packet[4] = (payload_length >> 8) & 0xFF;
    raw_packet[5] = payload_length & 0xFF;
    compute_checksum(raw_packet + 2, 4, raw_packet + 6);
    return 8;
}

/**
 * Encode a Lithium packet's body, assuming the header already exists
 *
 * @param payload The payload to encode
 * @param length The length of the payload
 * @param raw_packet The output byte array of at least 265 bytes
 *
 * @return The size of the packet body in bytes
 */
uint16_t encode_body(void * payload, uint16_t length, uint8_t * raw_packet) {
    memcpy(raw_packet + 8, payload, length);
    compute_checksum(raw_packet + 2, 6 + length, raw_packet + 8 + length);
    return length + 2;
}

/**
 * Send a byte array through the UART channel associated with a radio
 *
 * @param radio The radio instance owning the UART channel
 * @param raw_packet The data to send
 * @param length The length of the data to send
 *
 * @return The result of the operation
 */
lithium_result_t send_data(lithium_t * radio, uint8_t * raw_packet, uint16_t length) {
    uart_error_t err = uart_write_bytes(&radio->uart, raw_packet, length);
    if (err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }

    return LITHIUM_NO_ERROR;
}

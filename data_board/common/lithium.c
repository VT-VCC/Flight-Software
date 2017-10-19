#include <string.h>
#include "lithium.h"

#include "lithium_internal.h"

/******************************************************************************\
 *  Private support functions                                                 *
\******************************************************************************/
void compute_checksum(uint8_t * data, size_t len, uint8_t * output);
lithium_result_t lithium_send_header(lithium_t * radio, lithium_command_t command, uint16_t size);

/******************************************************************************\
 *  Pulic interface implementations                                           *
\******************************************************************************/

bool lithium_open(lithium_t * out, uart_t * uart) {
    out->uart = *uart;
    return true;
}

void lithium_close(lithium_t * out) {
    uart_close(&out->uart);
}

lithium_result_t lithium_send_packet(lithium_t * radio, lithium_packet_t * packet) {
    //TODO check if I/O message

    lithium_result_t res = lithium_send_header(radio, packet->command, packet->payload_length);

    // Return immediately if there is no payload or sending the header failed
    if (res != LITHIUM_NO_ERROR || packet->payload_length == 0) {
        return res;
    }
    else {
        // Create packet body including the payload and checksum
        uint16_t body_length = packet->payload_length + 2;
        uint8_t body[255/*payload*/ + 2*1/*checksum*/];

        // Copy payload into body, followed by the calculated checksum
        memcpy(body, packet->payload, packet->payload_length);
        compute_checksum(body, packet->payload_length, body + packet->payload_length);

        uart_error_t err = uart_write_bytes(&radio->uart, body, body_length);
        if (err != UART_NO_ERROR) {
            return LITHIUM_BAD_COMMUNICATION;
        }
        else {
            return LITHIUM_NO_ERROR;
        }
    }
}

/* Commands with no payload */

#define EMIT_SEND(name, cmd) \
    lithium_result_t lithium_send_##name(lithium_t * radio) { \
        return lithium_send_header(radio, LITHIUM_COMMAND_##cmd, 0); \
    }

EMIT_SEND(no_op, NO_OP)
EMIT_SEND(reset, RESET_SYSTEM)
EMIT_SEND(get_config, GET_TRANSCEIVER_CONFIG)
EMIT_SEND(query_telem, TELEMETRY_QUERY)
EMIT_SEND(read_fw_version, READ_FIRMWARE_REVISION)

#undef EMIT_SEND

/* Commands with a payload */

#define EMIT_SEND_PAYLOAD(name, cmd, data_param, length_param, ...) \
    lithium_result_t lithium_send_##name(lithium_t * radio, __VA_ARGS__) { \
        lithium_packet_t packet = { \
            .command = LITHIUM_COMMAND_##cmd, \
            .payload_length = length_param \
        }; \
        memcpy(packet.payload, data_param, packet.payload_length); \
        return lithium_send_packet(radio, &packet); \
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
    &speed, 1,
    uint8_t speed)

#undef EMIT_SEND_PAYLOAD

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

lithium_result_t lithium_send_header(lithium_t * radio, lithium_command_t command, uint16_t size) {
    uint8_t header[8];
    header[0] = SYNC_1;
    header[1] = SYNC_2;
    header[2] = I_MESSAGE_TYPE;
    header[3] = command;
    header[4] = (size >> 8) & 0xFF;
    header[5] = size & 0xFF;

    // Calculate checksum on bytes [2, 6)
    compute_checksum(header + 2, 6, header + 6);

    uart_error_t err = uart_write_bytes(&radio->uart, header, 8);
    if (err != UART_NO_ERROR) {
        return LITHIUM_BAD_COMMUNICATION;
    }
    else {
        return LITHIUM_NO_ERROR;
    }
}

/**
 * Packet intrinsics
 */

// First packet header magic byte
#define SYNC_1 'H'
// Second packet header magic byte
#define SYNC_2 'e'
// The command type of an I-Message
#define I_MESSAGE_TYPE 0x10
// The command type of an O-Message
#define O_MESSAGE_TYPE 0x20
// The payload length of an ACK O-Message
#define ACK_LENGTH    0x0a0a
// The payload length of a NACK O-Message
#define NACK_LENGTH   0xffff

/**
 * Upper-bound packet sizes
 */

// Length of the sync bytes
#define SYNC_BYTES_LENGTH   2
// Length of the checksumed command and payload length header bytes
#define HEADER_DATA_LENGTH  4
// Length of the header/payload checksum bytes
#define CHECKSUM_LENGTH     2
// Length of all of the header bytes
#define HEADER_LENGTH       SYNC_BYTES_LENGTH + HEADER_DATA_LENGTH + CHECKSUM_LENGTH
// Maximum length of the payload
#define MAX_PAYLOAD_LENGTH  255
// Maximum length of the entire packet
#define MAX_PACKET_LENGTH   HEADER_LENGTH + MAX_PAYLOAD_LENGTH + CHECKSUM_LENGTH

/**
 * Over-the-air commands
 */

#define TELEMETRY_DUMP_COMMAND  0x30
#define PING_RETURN_COMMAND     0x31
#define CODE_UPLOAD_COMMAND     0x32
#define RADIO_RESET_COMMAND     0x33
#define PIN_TOGGLE_COMMAND      0x34

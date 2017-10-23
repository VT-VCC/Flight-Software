// Packet intrinsics
#define SYNC_1 'H'
#define SYNC_2 'e'
#define I_MESSAGE_TYPE 0x10
#define O_MESSAGE_TYPE 0x20

#define ACK_SIZE    0x0a0a
#define NACK_SIZE   0xffff

#define TELEMETRY_DUMP_COMMAND  0x30
#define PING_RETURN_COMMAND     0x31
#define CODE_UPLOAD_COMMAND     0x32
#define RADIO_RESET_COMMAND     0x33
#define PIN_TOGGLE_COMMAND      0x34

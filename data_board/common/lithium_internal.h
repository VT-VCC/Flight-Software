//Configuration Commands
#define NO_OP_COMMAND           0x01
#define RESET_SYSTEM            0x02
#define TRANSMIT_DATA           0x03
#define RECEIVE_DATA            0x04
#define GET_TRANSCEIVER_CONFIG  0x05
#define SET_TRANSCEIVER_CONFIG  0x06
#define TELEMETRY_QUERY         0x07
#define WRITE_FLASH             0x08
#define RF_CONFIG               0x09
#define BEACON_DATA             0x10
#define BEACON_CONFIG           0x11
#define READ_FIRMWARE_REVISION  0x12
#define WRITE_OVER_AIR_KEY      0x13
#define FIRMWARE_UPDATE         0x14
#define FIRMWARE_PACKET         0x15
#define FAST_PA_SET             0x20

#define BAUD_RATE_9600      0
#define BAUD_RATE_19200     1
#define BAUD_RATE_38400     2
#define BAUD_RATE_76800     3
#define BAUD_RATE_115200    4

#define RF_BAUD_RATE_1200   0
#define RF_BAUD_RATE_9600   1
#define RF_BAUD_RATE_19200  2
#define RF_BAUD_RATE_38400  3

#define RF_MODULATION_GFSK  0
#define RF_MODULATION_AFSK  1
#define RF_MODULATION_BPSK  2


#define TELEMETRY_DUMP_COMMAND  0x30
#define PING_RETURN_COMMAND     0x31
#define CODE_UPLOAD_COMMAND     0x32
#define RADIO_RESET_COMMAND     0x33
#define PIN_TOGGLE_COMMAND      0x34

#ifndef _COMMON_MMC_INTERNAL_H_
#define _COMMON_MMC_INTERNAL_H_

// Default size of a sector
#define MMC_SECTOR_SIZE 512

/// Commands
#define MMC_GO_IDLE_STATE 0
#define MMC_SEND_OP_COND 1
#define MMC_SWITCH 6
#define MMC_SEND_EXT_CSD 8
#define MMC_SEND_CSD 9
#define MMC_SEND_CID 10
#define MMC_STOP_TRANSMISSION 12
#define MMC_SEND_STATUS 13
#define MMC_SET_BLOCKLEN 16
#define MMC_READ_SINGLE_BLOCK 17
#define MMC_READ_MULTIPLE_BLOCK 18
#define MMC_SET_BLOCK_COUNT 23
#define MMC_WRITE_BLOCK 24
#define MMC_WRITE_MULTIPLE_BLOCK 25
#define MMC_PROGRAM_CSD 27
#define MMC_SET_WRITE_PROT 28
#define MMC_CLR_WRITE_PROT 29
#define MMC_SEND_WRITE_PROT 30
#define MMC_ERASE_GROUP_START 35
#define MMC_ERASE_GROUP_END 36
#define MMC_ERASE 38
#define MMC_LOCK_UNLOCK 42
#define MMC_APP_CMD 55
#define MMC_GEN_CMD 56
#define MMC_READ_OCR 58
#define MMC_CRC_ON_OFF 59

// Mask sent while waiting for a response
#define MMC_BUSY_MASK 0x80

/// Response error codes
#define MMC_SUCCESS           0x00
#define MMC_BLOCK_SET_ERROR   0x01
#define MMC_RESPONSE_ERROR    0x02
#define MMC_DATA_TOKEN_ERROR  0x03
#define MMC_INIT_ERROR        0x04
#define MMC_CRC_ERROR         0x10
#define MMC_WRITE_ERROR       0x11
#define MMC_OTHER_ERROR       0x12

/// Tokens
#define MMC_START_DATA_BLOCK_TOKEN          0xfe   // Data token start byte, Start Single Block Read
#define MMC_START_DATA_MULTIPLE_BLOCK_READ  0xfe   // Data token start byte, Start Multiple Block Read
#define MMC_START_DATA_BLOCK_WRITE          0xfe   // Data token start byte, Start Single Block Write
#define MMC_START_DATA_MULTIPLE_BLOCK_WRITE 0xfc   // Data token start byte, Start Multiple Block Write
#define MMC_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xfd   // Data toke stop byte, Stop Multiple Block Write

#endif // _COMMON_MMC_INTERNAL_H_

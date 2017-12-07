#ifndef _COMMON_RFM_H_
#define _COMMON_RFM_H_

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The connection to an RFM radio
 */
typedef struct rfm {
    /**
     * The owned SPI channel over which we talk
     */
    spi_t spi;
} rfm_t;

/**
 * Macro list for results of operations on the RFM
 */
#define RFM_RESULT_LIST(OP) \
    OP(NO_ERROR) \
    OP(SPI_ERROR)

/**
 * Enumeration of possible results for trying to communicate with the RFM
 */
typedef enum rfm_result {
#   define STRING_OP(E) RFM_ ## E,
    RFM_RESULT_LIST(STRING_OP)
#   undef STRING_OP
    RFM_count
} rfm_result_t;

#undef RFM_RESULT_LIST

/**
 * Enumeration of transceiver operating modes. Values correspond to RegOpModes
 */
typedef enum rfm_mode {
    RFM_MODE_SLEEP = (0x00 << 2),
    RFM_MODE_STDBY = (0x01 << 2),
    RFM_MODE_FS = (0x02 << 2),
    RFM_MODE_TX = (0x03 << 2),
    RFM_MODE_RX = (0x04 << 2)
} rfm_mode_t;

/**
 * Open a connection to an RFM radio
 *
 * @param radio The output radio object
 * @param spi The SPI channel to use. Takes ownership of the lifecycle of this
 *      channel
 *
 * @return True if and only if the radio connection succeeds
 */
bool rfm_open(rfm_t * radio, spi_t * spi);

/**
 * Close a connection to an RFM radio
 *
 * @param radio The radio to close
 */
void rfm_close(rfm_t * radio);

rfm_result_t rfm_read_reg(rfm_t * radio, uint8_t addr, uint8_t * value);

rfm_result_t rfm_write_reg(rfm_t * radio, uint8_t addr, uint8_t value);

rfm_result_t rfm_read_fifo(rfm_t * radio, uint8_t * data, uint8_t size);

rfm_result_t rfm_write_fifo(rfm_t * radio, uint8_t * data, uint8_t size);

rfm_result_t rfm_set_frequency(rfm_t * radio, uint32_t frequency);

rfm_result_t rfm_get_frequency(rfm_t * radio, uint32_t * frequency);

rfm_result_t rfm_set_mode(rfm_t * radio, rfm_mode_t mode);

rfm_result_t rfm_set_address(rfm_t * radio, uint8_t address);

rfm_result_t rfm_set_sync_word(rfm_t * radio, uint8_t * sync_word, uint8_t length);

rfm_result_t rfm_set_power_level(rfm_t * radio, uint8_t level);

#ifdef __cplusplus
}
#endif

#endif // _COMMON_RFM_H_

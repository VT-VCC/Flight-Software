#ifndef _BOARD_COMMON_MMC_H_
#define _BOARD_COMMON_MMC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************\
 *  Macro list for the ways writing to an MMC can fail                        *
\******************************************************************************/
/// Macro for defining thing related to MMC errors
#define MMC_ERROR_LIST(OP) \
    OP(NO_ERROR) \
    OP(BAD_RESPONSE) \
    OP(TIMEOUT)

/// Enum representing possible error states for an MMC device
typedef enum mmc_error {
#   define ENUM_OP(E) MMC_ ## E,
    MMC_ERROR_LIST(ENUM_OP)
#   undef ENUM_OP
    mmc_count
} mmc_error_t;

/**
 * The connection to an MMC device
 */
typedef struct mmc {
    /**
     * The aliased SPI channel over which we talk
     */
    spi_t * spi;
    volatile uint8_t *cs_value;
    uint8_t cs_bit;
} mmc_t;

bool mmc_init(mmc_t * device, spi_t * spi, volatile uint8_t *cs_value, uint8_t cs_bit);

/*
mmc_error_t mmc_read_register(mmc_t * device, uint8_t addr, uint8_t len, uint8_t * block);
*/

mmc_error_t mmc_set_block_len(mmc_t * device, uint32_t len);

mmc_error_t mmc_idle(mmc_t * device);

mmc_error_t mmc_read_block(mmc_t * device, uint32_t addr, uint32_t len, uint8_t * block);

mmc_error_t mmc_write_block(mmc_t * device, uint32_t addr, uint32_t len, uint8_t * block);

mmc_error_t mmc_read_sector(mmc_t * device, uint32_t index, uint8_t * sector);

mmc_error_t mmc_write_sector(mmc_t * device, uint32_t index, uint8_t * sector);

/** @} */

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_MMC_H_

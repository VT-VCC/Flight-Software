#ifndef _BOARD_COMMON_NATIVE_SPI_USCI_H_
#define _BOARD_COMMON_NATIVE_SPI_USCI_H_

#include "usci.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spi {
    /**
     * The USCI module we are going to use
     */
    usci_t usci;
} spi_t;

/**
 * Open a connection to an SPI channel
 *
 * @param usci The USCI channel to use
 * @param clock_rate The clock rate at which we will run
 * @param out The SPI structure to fill
 *
 */
bool spi_open(usci_t usci, uint32_t clock_rate, spi_t * out);

#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_SPI_USCI_H_

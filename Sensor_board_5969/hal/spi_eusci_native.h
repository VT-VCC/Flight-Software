#ifndef _BOARD_COMMON_NATIVE_SPI_EUSCI_H_
#define _BOARD_COMMON_NATIVE_SPI_EUSCI_H_

#include "eusci.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct spi {
    /**
     * Which EUSCI module this SPI is connected to
     */
    eusci_t eusci;
} spi_t;

/**
 * Open a connection to a SPI channel
 *
 * @param eusci The EUSCI channel to use
 * @param clock_rate The clock rate at which we will run
 * @param out The SPI structure to fill
 * ju
 */
bool spi_open(eusci_t eusci, uint32_t clock_rate, spi_t * out);
    
#ifdef __cplusplus
}
#endif

#endif // _BOARD_COMMON_NATIVE_SPI_EUSCI_H_

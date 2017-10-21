if (${MSP_SYSTEM_CLASS} STREQUAL MSP430_F5xx_6xx)
  add_sources(
    BOARD_COMMON_SOURCES
    "uart_usci_native.h"
    "uart_usci_native.c"
    "spi_usci_native.h"
    "spi_usci_native.c"
  )
else()
  add_sources(
    BOARD_COMMON_SOURCES
    "uart_eusci_native.h"
    "uart_eusci_native.c"
    "spi_eusci_native.h"
    "spi_eusci_native.c"
  )
endif()

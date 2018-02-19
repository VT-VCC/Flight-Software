if (${MSP_SYSTEM_CLASS} STREQUAL MSP430_F5xx_6xx)
  add_sources(
    BOARD_COMMON_SOURCES
    "i2c_usci_native.c"
    "spi_usci_native.c"
    "uart_usci_native.c"
  )
else()
  add_sources(
    BOARD_COMMON_SOURCES
    "i2c_eusci_native.c"
    "spi_eusci_native.c"
    "uart_eusci_native.c"
  )
endif()

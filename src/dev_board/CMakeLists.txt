add_sources(DEV_BOARD_SOURCES "gcd.c")
add_sources(DEV_BOARD_NATIVE_SOURCES "main.c")

# MSP430 build
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  mcu_target(dev_board.out msp430fr5849 DEV_BOARD_SOURCES DEV_BOARD_NATIVE_SOURCES)
endif()

add_sources(DATA_BOARD_SOURCES "factorial.c")
add_sources(DATA_BOARD_NATIVE_SOURCES "main.c")

# MSP430 build
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  mcu_target(databoard.out msp430f5438a DATA_BOARD_SOURCES DATA_BOARD_NATIVE_SOURCES)
endif()

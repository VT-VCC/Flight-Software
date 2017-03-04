add_sources(SENSOR_BOARD_SOURCES "gcd.c")
add_sources(SENSOR_BOARD_NATIVE_SOURCES "main.c")

# MSP430 build
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  mcu_target(sensor_board.out msp430fr5849 SENSOR_BOARD_SOURCES SENSOR_BOARD_NATIVE_SOURCES)
endif()

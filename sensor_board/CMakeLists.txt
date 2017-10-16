# common modules
add_subdirectory(common)
include_directories("${CMAKE_SOURCE_DIR}/common")

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)

  add_msp430_executable(sensor_board SENSOR_BOARD_SOURCES)
  # target_compile_options(sensor_board PRIVATE -Wall)
  # target_compile_options(sensor_board PRIVATE -Wextra)
else()
  # test build
  add_subdirectory(test)
endif()

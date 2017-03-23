# common modules
add_subdirectory(common)
include_directories("${CMAKE_SOURCE_DIR}/common")

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)

  get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)

  add_executable(data_board ${DATA_BOARD_SOURCES})
else()
  # test build
  add_subdirectory(test)
endif()

cmake_minimum_required(VERSION 3.5)

# Set the project name
project(VT_USIP_FLIGHT_SOFTWARE LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(usip_macros)

# third-party libraries
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty")

# common modules
add_subdirectory(board_common)
include_directories("${CMAKE_SOURCE_DIR}/board_common/common")

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  if (USIP_DATA_BOARD)
    add_subdirectory(data_board)
  endif()
  if (USIP_DEV_BOARD)
    add_subdirectory(dev_board)
  endif()
  if (USIP_SENSOR_BOARD)
    add_subdirectory(sensor_board)
  endif()
else()
  # test build
  enable_language(CXX)
  add_subdirectory(data_board)
  add_subdirectory(dev_board)
  add_subdirectory(sensor_board)

  get_property(DEV_BOARD_SOURCES GLOBAL PROPERTY DEV_BOARD_SOURCES)
  get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)
  get_property(SENSOR_BOARD_SOURCES GLOBAL PROPERTY SENSOR_BOARD_SOURCES)
  get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)

  include_directories(data_board/common)
  include_directories(dev_board/common)
  include_directories(sensor_board/common)

  add_executable(usip_test
    ${DEV_BOARD_SOURCES}
    ${DATA_BOARD_SOURCES}
    ${SENSOR_BOARD_SOURCES}
    ${BOARD_COMMON_SOURCES}
  )
endif()

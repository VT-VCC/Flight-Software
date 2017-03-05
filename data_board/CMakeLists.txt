cmake_minimum_required(VERSION 3.5)

# Prevent in-source builds
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "In-source builds are not supported.")
endif()

# Set the project name
project(VT_USIP_DATA_BOARD LANGUAGES C)

# Load external CMAKE stuff
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/../cmake")
include(usip_macros)

# third-party libraries
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/../3rdparty")

# common modules
add_subdirectory(common)
include_directories("${CMAKE_SOURCE_DIR}/common")

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)
else()
  # test build
  enable_language(CXX)
  add_subdirectory(test)
endif()

get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)

add_executable(data_board ${DATA_BOARD_SOURCES})

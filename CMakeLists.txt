cmake_minimum_required(VERSION 3.5)

# Set the project name
project(VT_USIP_FLIGHT_SOFTWARE LANGUAGES C)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(usip_macros)

# third-party libraries
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty")

# select the right MSP430 support library
if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430F5xx_6xx
  if(${MSP430_MCU} MATCHES "msp430f5.*" OR ${MSP430_MCU} MATCHES "msp430f6.*")
    include_directories(SYSTEM "3rdparty/msp430_driverlib/MSP430F5xx_6xx")
    add_subdirectory("3rdparty/msp430_driverlib/MSP430F5xx_6xx")
    add_definitions(-DMSP430_CLASS_F5xx_6xx)
    set(MSP_SYSTEM_CLASS MSP430_F5xx_6xx)
  # MSP430FR2xx_4xx
  elseif(${MSP430_MCU} MATCHES "msp430f2.*" OR ${MSP430_MCU} MATCHES "msp430f4.*")
    include_directories(SYSTEM "$3rdparty/msp430_driverlib/MSP430F2xx_4xx")
    add_subdirectory("3rdparty/msp430_driverlib/MSP430F2xx_4xx")
    add_definitions(-DMSP430_CLASS_F2xx_4xx)
    set(MSP_SYSTEM_CLASS MSP430_F2xx_4xx)
  # MSP430FR57xx
  elseif(${MSP430_MCU} MATCHES "msp430fr57.*")
    include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/msp430_driverlib/MSP430FR57xx")
    add_definitions(-DMSP430_CLASS_FR57xx)
    set(MSP_SYSTEM_CLASS MSP430_FR57xx)
  # MSP430FR5xx_6xx
  elseif(${MSP430_MCU} MATCHES "msp430fr5.*" OR ${MSP430_MCU} MATCHES "msp430fr6.*")
    include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/msp430_driverlib/MSP430FR5xx_6xx")
    add_subdirectory("3rdparty/msp430_driverlib/MSP430FR5xx_6xx")
    add_definitions(-DMSP430_CLASS_FR5xx_6xx)
    set(MSP_SYSTEM_CLASS MSP430_CLASS_6xx)
  # MSP430i2xx
  elseif(${MSP430_MCU} MATCHES "msp430i2.*")
    include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/msp430_driverlib/MSP430i2xx")
    add_definitions(-DMSP430_CLASS_I2xx)
    set(MSP_SYSTEM_CLASS MSP430_I2xx)
  else()
    message(FATAL_ERROR "Unknown MSP430 MCU ${MSP430_MCU}")
  endif()
  # freertos includes
  include_directories(SYSTEM "3rdparty/freertos")
  include_directories(SYSTEM "3rdparty/freertos/include")
  include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty/freertos/portable/MSP430X")
endif()

# common modules
include_directories("${CMAKE_SOURCE_DIR}/board_common/common")

add_subdirectory(board_common)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # For msp430 builds, use LTO with size optimization
  # We use GCC flag syntax here since we always know we're using msp430-elf-gcc
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99 -flto -fno-use-linker-plugin -Os")
  message(STATUS "Performing MSP430 build")
  add_definitions(-DUSIP_NATIVE)
  include_directories(board_common/native)
  # MSP430 build
  if (USIP_DATA_BOARD)
    message(STATUS "Building data board")
    add_subdirectory(data_board)
  endif()
  if (USIP_DEV_BOARD)
    message(STATUS "Building dev board")
    include_directories(dev_board/native)
    include_directories(dev_board/common)

    add_subdirectory(dev_board)
  endif()
  if (USIP_SENSOR_BOARD)
    message(STATUS "Building sensor board")
    include_directories(sensor_board/native)
    include_directories(sensor_board/common)

    add_subdirectory(sensor_board)
  endif()

  # freertos build
  add_subdirectory("3rdparty/freertos/")
else()
  # test build
  message(STATUS "Building test binary")
  # Require C++11
  enable_language(CXX)
  set(CMAKE_CXX_STANDARD 11)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  add_subdirectory(data_board)
  add_subdirectory(dev_board)
  add_subdirectory(sensor_board)

  include_directories(data_board/common)
  include_directories(dev_board/common)
  include_directories(sensor_board/common)
  include_directories(board_common/test/impl)

  get_property(DEV_BOARD_SOURCES GLOBAL PROPERTY DEV_BOARD_SOURCES)
  get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)
  get_property(SENSOR_BOARD_SOURCES GLOBAL PROPERTY SENSOR_BOARD_SOURCES)
  get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)

  add_executable(usip_test
    ${DEV_BOARD_SOURCES}
    ${DATA_BOARD_SOURCES}
    ${SENSOR_BOARD_SOURCES}
    ${BOARD_COMMON_SOURCES}
  )
endif()

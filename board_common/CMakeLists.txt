# This builds a static library containing common code for all of the flight
# boards.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

add_subdirectory(common)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)

  get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)

  add_library(vt_usip_common STATIC ${BOARD_COMMON_SOURCES})
else()
  # test build
  enable_language(CXX)
  add_subdirectory(test)

  get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)
  get_property(BOARD_COMMON_TEST_SOURCES GLOBAL PROPERTY BOARD_COMMON_TEST_SOURCES)

  add_library(vt_usip_common STATIC ${BOARD_COMMON_SOURCES})

  add_executable(vt_usip_common_test ${BOARD_COMMON_TEST_SOURCES} ${BOARD_COMMON_SOURCES})
endif()

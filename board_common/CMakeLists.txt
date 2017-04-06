# This builds a static library containing common code for all of the flight
# boards.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

add_subdirectory(common)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)
else()
  # test build
  add_subdirectory(test)
endif()

# generate static library
get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)
add_library(vt_usip_common STATIC ${BOARD_COMMON_SOURCES})

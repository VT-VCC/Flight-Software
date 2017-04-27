# This builds a static library containing common code for all of the flight
# boards.
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/common")

# common sources
add_subdirectory(common)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  message(STATUS "Building vt_usip_common for msp430")
  # MSP430 build
  add_definitions(-DUSIP_NATIVE)
  add_subdirectory(native)
  include_directories(native)
else()
  message(STATUS "Building vt_usip_common for host")
  # test build
  add_subdirectory(test)
  include_directories(test)
  include_directories(test/impl)

  enable_language(CXX)
endif()

# generate static library
get_property(BOARD_COMMON_SOURCES GLOBAL PROPERTY BOARD_COMMON_SOURCES)
add_library(vt_usip_common STATIC ${BOARD_COMMON_SOURCES})
# target_compile_options(vt_usip_common PRIVATE -Wall)
# target_compile_options(vt_usip_common PRIVATE -Wextra)

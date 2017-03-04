cmake_minimum_required(VERSION 3.7)

# Prevent in-source builds
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "In-source builds are not supported.")
endif()

# Set the project name
project(VT_USIP)

# Load external CMAKE stuff
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(usip_macros)

# set up include paths
include_directories(SYSTEM "${CMAKE_SOURCE_DIR}/3rdparty")
include_directories("${CMAKE_SOURCE_DIR}/src")

# recurse in to subdirectories
add_subdirectory(src)

# must be after all source directories so source list is set up
# We only build tests when building on the host platform
if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL Generic)
  message(STATUS "We will build the test binary")
  add_subdirectory(test)
elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  message(STATUS "Test binary will not be built, targetting MSP430")
else()
  message(FATAL_ERROR "Unsupported processor ${CMAKE_SYSTEM_PROCESSOR}")
endif()

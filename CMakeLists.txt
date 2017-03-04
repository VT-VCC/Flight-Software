cmake_minimum_required(VERSION 3.7)

# Prevent in-source builds
if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR, "In-source builds are not supported.")
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
add_subdirectory(test) 

# common modules
add_subdirectory(common)
include_directories("${CMAKE_SOURCE_DIR}/common")

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)

  add_msp430_executable(dev_board DEV_BOARD_SOURCES)

  target_link_libraries(dev_board vt_usip_common)
  target_link_libraries(dev_board msp430_driverlib)
  target_link_libraries(dev_board freertos)
  # target_compile_options(dev_board PRIVATE -Wall)
  # target_compile_options(dev_board PRIVATE -Wextra)
else()
  # test build
  add_subdirectory(test)
endif()

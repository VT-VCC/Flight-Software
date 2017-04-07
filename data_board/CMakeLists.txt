# common modules
add_subdirectory(common)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL msp430)
  # MSP430 build
  add_subdirectory(native)

  get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)

  add_executable(data_board ${DATA_BOARD_SOURCES})

  target_link_libraries(data_board vt_usip_common)
  target_include_directories(data_board PRIVATE native)
  target_include_directories(data_board PUBLIC common)
else()
  # test build
  add_subdirectory(test)
endif()

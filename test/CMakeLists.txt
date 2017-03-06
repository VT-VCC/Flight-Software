add_subdirectory(data_board)
add_Subdirectory(sensor_board)

# get the list of test sources
get_property(TEST_SOURCES GLOBAL PROPERTY TEST_SOURCES)

# get the list of platform-independent sources for the data and sensor boards
get_property(DATA_BOARD_SOURCES GLOBAL PROPERTY DATA_BOARD_SOURCES)
get_property(SENSOR_BOARD_SOURCES GLOBAL PROPERTY SENSOR_BOARD_SOURCES)

add_executable(usip_test
    test_driver.cpp
    ${TEST_SOURCES}
    ${DATA_BOARD_SOURCES}
    ${SENSOR_BOARD_SOURCES}
)

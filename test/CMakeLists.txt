set(TEST_SOURCES
  "factorial.cpp"
  "test_driver.cpp"
)

get_property(DATABOARD_SOURCES GLOBAL PROPERTY DATABOARD_SOURCES)

add_executable(usip_test ${TEST_SOURCES} ${DATABOARD_SOURCES})

#include <catch/catch.hpp>

#include "impl/uart.hpp"

#include "uart.h"

TEST_CASE("Test UART implementation can push bytes", "[uart]") {
    uart_t t;
    uint8_t test_data[3] = { 0x02, 0x03, 0x04 };

    REQUIRE(uart_open(&t, 9600));

    REQUIRE(uart_write_byte(&t, 0x01) == UART_NO_ERROR);
    REQUIRE(uart_write_bytes(&t, test_data, 3) == UART_NO_ERROR);

    REQUIRE_THAT(t, HasWrittenBytes({0x01, 0x02, 0x03, 0x04}));
}

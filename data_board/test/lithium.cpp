#include "lithium.h"

#include <catch/catch.hpp>

TEST_CASE("The radio interface can send noops", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    REQUIRE(lithium_send_noop(&t) == LITHIUM_NO_ERROR);

    REQUIRE_THAT(t.uart, HasWrittenBytes({
        0x48, 0x65, // Synchronization bytes
        0x10, 0x01, // Command
        0x00, 0x00, // Length
        0xBE, 0xEC  // Checksum
    }));

    lithium_close(&t);
}

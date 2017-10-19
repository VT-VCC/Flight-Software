#include "lithium.h"

#include <catch/catch.hpp>

TEST_CASE("The radio interface can send noops", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    REQUIRE(lithium_send_no_op(&t) == LITHIUM_NO_ERROR);

    REQUIRE_THAT(t.uart, HasWrittenBytes({
        0x48, 0x65, // Synchronization bytes
        0x10, 0x01, // Command
        0x00, 0x00, // Length
        0x11, 0x65  // Checksum
    }));

    lithium_close(&t);
}

TEST_CASE("The radio interface can send reset requests", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    REQUIRE(lithium_send_reset(&t) == LITHIUM_NO_ERROR);

    REQUIRE_THAT(t.uart, HasWrittenBytes({
        0x48, 0x65, // Synchronization bytes
        0x10, 0x02, // Command
        0x00, 0x00, // Payload length
        0x12, 0x6a  // Header checksum
    }));

    lithium_close(&t);
}

TEST_CASE("The radio interface can send transmissions", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    SECTION("Data set 1") {
        uint8_t data[15] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

        REQUIRE(lithium_send_transmit(&t, data, 15) == LITHIUM_NO_ERROR);

        REQUIRE_THAT(t.uart, HasWrittenBytes({
            0x48, 0x65, // Synchronization bytes
            0x10, 0x03, // Command
            0x00, 0x0f, // Payload length
            0x22, 0x9c, // Header checksum
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, // Payload
            0x69, 0x30  // Payload checksum
        }));
    }

    SECTION("Data set 2") {
        uint8_t data[] = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
        };

        REQUIRE(lithium_send_transmit(&t, data, 64) == LITHIUM_NO_ERROR);

        REQUIRE_THAT(t.uart, HasWrittenBytes({
            0x48, 0x65, // Synchronization bytes
            0x10, 0x03, // Command
            0x00, 0x40, // Payload length
            0x53, 0x2f, // Header checksum
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20, 5,  9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21,
            0x50, 0x80  // Payload checksum
        }));
    }

    lithium_close(&t);
}

TEST_CASE("The radio interface can send power amplifier changes", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    REQUIRE(lithium_send_set_pa_level(&t, 5) == LITHIUM_NO_ERROR);

    REQUIRE_THAT(t.uart, HasWrittenBytes({
        0x48, 0x65, // Synchronization bytes
        0x10, 0x20, // Command
        0x00, 0x01, // Payload length
        0x31, 0x03, // Header checksum
        5,
        0x05, 0x05  // Payload checksum
    }));

    lithium_close(&t);
}

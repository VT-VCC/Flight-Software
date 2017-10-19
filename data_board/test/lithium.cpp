#include "uart.h"
#include "lithium.h"

#include <catch/catch.hpp>

// Set the input of the mock UART
#define SET_UART_INPUT(t, ...) \
    do { \
        t.uart._impl->input = { __VA_ARGS__ }; \
        std::reverse(t.uart._impl->input.begin(), t.uart._impl->input.end()); \
    } while (0)

// Check if a packet's payload has matching bytes
#define REQUIRE_PAYLOAD_HAS_BYTES(packet, ...) \
    do { \
        std::vector<uint8_t> vec(packet.payload, packet.payload + packet.payload_length); \
        REQUIRE(vec == std::vector<uint8_t>({ __VA_ARGS__ })); \
    } while (0)


TEST_CASE("The radio interface can handle no ops", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    SECTION("Write send") {
        REQUIRE(lithium_send_no_op(&t) == LITHIUM_NO_ERROR);

        REQUIRE_THAT(t.uart, HasWrittenBytes({
            0x48, 0x65,
            0x10, 0x01,
            0x00, 0x00,
            0x11, 0x43,
        }));
    }

    SECTION("Parse send") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x10, 0x01,
            0x00, 0x00,
            0x11, 0x43,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_I_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_NO_OP);
        REQUIRE(packet.payload_length == 0);
    }

    SECTION("Parse ACK receive") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x20, 0x01,
            0x0a, 0x0a,
            0x35, 0xa1,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_O_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_NO_OP);
        REQUIRE(lithium_is_ack(&packet));
    }

    SECTION("Parse NACK receive") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x20, 0x01,
            0xff, 0xff,
            0x1f, 0x80,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_O_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_NO_OP);
        REQUIRE(lithium_is_nack(&packet));
    }

    lithium_close(&t);
}

TEST_CASE("The radio interface can handle transmissions", "[data_board][lithium]") {
    lithium_t t;
    uart_t uart;
    uart_open(&uart, 9600);
    lithium_open(&t, &uart);

    SECTION("Write send") {
        uint8_t data[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        uint16_t length = 10;

        REQUIRE(lithium_send_transmit(&t, data, length) == LITHIUM_NO_ERROR);

        REQUIRE_THAT(t.uart, HasWrittenBytes({
            0x48, 0x65,
            0x10, 0x03,
            0x00, 0x0a,
            0x1d, 0x53,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            0xba, 0x41,
        }));
    }

    SECTION("Parse send") {
        SECTION("Valid") {
            SET_UART_INPUT(t,
                0x48, 0x65,
                0x10, 0x03,
                0x00, 0x0a,
                0x1d, 0x53,
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                0xba, 0x41,
            );

            lithium_packet_t packet;
            REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

            REQUIRE(packet.type == LITHIUM_I_MESSAGE);
            REQUIRE(packet.command == LITHIUM_COMMAND_TRANSMIT_DATA);
            REQUIRE(packet.payload_length == 10);
            REQUIRE_PAYLOAD_HAS_BYTES(packet,
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            );
        }

        SECTION("Bad header checksum") {
            SET_UART_INPUT(t,
                0x48, 0x65,
                0x10, 0x03,
                0x00, 0x0a,
                0x1e, 0x54,
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                0xba, 0x41,
            );

            lithium_packet_t packet;
            REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_INVALID_CHECKSUM);
        }

        SECTION("Invalid payload size") {
            SET_UART_INPUT(t,
                0x48, 0x65,
                0x10, 0x03,
                0x0b, 0x0a,
                0x9a, 0x49,
                0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
                0xba, 0x41,
            );

            lithium_packet_t packet;
            REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_INVALID_PACKET);
        }
    }

    SECTION("Parse ACK receive") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x20, 0x03,
            0x0a, 0x0a,
            0x37, 0xa7,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_O_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_TRANSMIT_DATA);
        REQUIRE(lithium_is_ack(&packet));
    }

    SECTION("Parse NACK receive") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x20, 0x03,
            0xff, 0xff,
            0x21, 0x86,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_O_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_TRANSMIT_DATA);
        REQUIRE(lithium_is_nack(&packet));
    }

    SECTION("Parse received data") {
        SET_UART_INPUT(t,
            0x48, 0x65,
            0x20, 0x04,
            0x00, 0x0a,
            0x2e, 0x96,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            0x83, 0x23,
        );

        lithium_packet_t packet;
        REQUIRE(lithium_receive_packet(&t, &packet) == LITHIUM_NO_ERROR);

        REQUIRE(packet.type == LITHIUM_O_MESSAGE);
        REQUIRE(packet.command == LITHIUM_COMMAND_RECEIVE_DATA);
        REQUIRE_PAYLOAD_HAS_BYTES(packet,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
        );
    }

    lithium_close(&t);
}

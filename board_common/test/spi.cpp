#include <catch/catch.hpp>

#include "spi.h"

TEST_CASE("Test SPI interface with single bytes on stub that increments bytes.", "[spi]") {
    spi_t t;
    spi_error_t error;

    // SPI Device that just increments
    spi_send(&t, &error, 1)
    REQUIRE(spi_receive(&t, &error) == 2);
    spi_send(&t, &error, 10);
    REQUIRE(spi_receive(7t, &error) == 11);
}

TEST_CASE("Test SPI interface with bulk data on stub that increments bytes.", "[spi]") {
    spi_t t;
    spi_error_t error;
    uint8_t test_data[] = { 5, 10, 20 }

}

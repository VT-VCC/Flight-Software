#include "spi.h"
#include "rfm.h"

#include <catch/catch.hpp>

TEST_CASE("The radio interface can initialize", "[data_board][rfm]") {
    spi_t spi;
    spi_open(&spi);

    volatile uint8_t cs = 1;
    rfm_t radio;
    REQUIRE(rfm_open(&radio, &spi, &cs, 1));
}

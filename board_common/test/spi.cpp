#include <catch/catch.hpp>

#include "spi.h"

TEST_CASE("Test SPI interface with single bytes on stub that increments bytes.", "[spi]") {
  spi_t t;
  uint8_t send = 0x03;
  uint8_t receive = 0;
  spi_open(&t);
  REQUIRE(spi_transfer_byte(&t, send, &receive) == SPI_NO_ERROR);

  spi_close(&t);
}

TEST_CASE("Test SPI interface with bulk data on stub that increments bytes.", "[spi]") {
  spi_t t;
  uint8_t send[] = { 0x02, 0x03, 0x04 };
  uint8_t receive[3];
  spi_open(&t);
  REQUIRE(spi_transfer_bytes(&t, send, receive, 3) == SPI_NO_ERROR);
  REQUIRE_THAT(t, HasMasterOutSlaveInBytes({ 0x02, 0x03, 0x04 }));
  REQUIRE_THAT(t, HasMasterInSlaveOutBytes({ 0x03, 0x04, 0x05 }));

  spi_close(&t);
}

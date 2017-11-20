#ifndef _TEST_SPI_HPP_
#define _TEST_SPI_HPP_

#include "spi.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct spi_impl spi_impl_t;

    /// A physical type for the SPI type.
    /// Essentially just a pointer to a C++ implementation
    typedef struct spi {
        spi_impl_t * _impl;
    } spi_t;
#ifdef __cplusplus
}

#include <catch/catch.hpp>
#include <vector>

struct spi_impl {
  std::vector<uint8_t> miso_bytes;
  std::vector<uint8_t> mosi_bytes;
  bool open;

  void send_byte();

  spi_impl() : open(false) {}
};

/** Open the given SPI channel so that it can be used.
 * @param The SPI channel to open.
 */
void spi_open(spi_t * out);

std::ostream & operator<<(std::ostream & o, const spi_t & t);
std::ostream & operator<<(std::ostream & o, const spi_error_t & err);

#endif

#endif // _TEST_SPI_HPP_

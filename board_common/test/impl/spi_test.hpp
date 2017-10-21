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

#endif

#endif // _TEST_SPI_HPP_

#ifndef _TEST_I2C_HPP_
#define _TEST_I2C_HPP_

#include "i2c.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct i2c_impl i2c_impl_t;

    /// A physical type for the I2C type.
    /// Essentially just a pointer to a C++ implementation
    typedef struct i2c {
        i2c_impl_t * _impl;
    } i2c_t;
#ifdef __cplusplus
}

#include <catch/catch.hpp>
#include <vector>

struct i2c_impl {
  bool open;

  void send_byte();

  i2c_impl() : open(false) {}
};

/** Open the given I2C channel so that it can be used.
 * @param The I2C channel to open.
 */
void i2c_open(i2c_t * out);

#endif

#endif // _TEST_I2C_HPP_

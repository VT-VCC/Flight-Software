#ifndef _TEST_UART_HPP_
#define _TEST_UART_HPP_

#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif
    typedef struct uart_impl uart_impl_t;

    /// A physical type for the UART type.
    /// Essentially just a pointer to a C++ implementation
    typedef struct uart {
        uart_impl_t * _impl;
    } uart_t;
#ifdef __cplusplus
}

#include <catch/catch.hpp>
#include <vector>

/// Implementation of the UART structure for testing infrastructure
struct uart_impl {
    /// Bytes that have been pushed in to the output by the uart client functions
    std::vector<uint8_t> output;
    /// Bytes waiting to be read in
    std::vector<uint8_t> input;
    /// True if we've opened
    bool open;

    /// Push a byte for consumption upon the next call to read_byte
    void push_byte(const uint8_t b);

    template<class InputIt> void push_bytes(InputIt first, InputIt last) {
        input.insert(input.begin(), first, last);
    }

    void push_bytes(std::initializer_list<uint8_t> data);

    uart_impl() : open(false) {}
};

bool uart_open(uart_t * out, size_t baud_rate);

/******************************************************************************\
 *  Pretty printers                                                           *
\******************************************************************************/
std::ostream & operator<<(std::ostream & o, const uart_t & t);
std::ostream & operator<<(std::ostream & o, const uart_error_t & err);

/******************************************************************************\
 *  UART matchers                                                             *
\******************************************************************************/
class HasWrittenBytes : public Catch::MatcherBase<uart_t> {
    private:
        std::vector<uint8_t> _bytes;
    public:
        HasWrittenBytes(const std::initializer_list<uint8_t> data);
        HasWrittenBytes(const std::vector<uint8_t> data);

        virtual bool match(const uart_t & e) const override;
        virtual std::string describe() const override;
};
#endif

#endif // _TEST_UART_HPP_

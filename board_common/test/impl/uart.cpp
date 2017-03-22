#include "uart.hpp"

/******************************************************************************\
 *  UART structure implementation                                             *
\******************************************************************************/
void uart::push_byte(const uint8_t b) {
    input.insert(input.begin(), b);
}

void uart::push_bytes(std::initializer_list<uint8_t> data) {
    input.insert(input.begin(), data);
}

/******************************************************************************\
 *  Pretty printers                                                           *
\******************************************************************************/
std::ostream & operator<<(std::ostream & o, const uart_t & t) {
    o << "{uart: output {";
    auto f = o.flags();

    o.setf(std::ios::hex);
    for (const auto & i : t.output) {
        o << " " << std::setw(2) << std::setfill('0') << (int)i;
    }

    o.flags(f);

    o << " } }";
    return o;
}

std::ostream & operator<<(std::ostream & o, const uart_error_t & err) {
    o << uart_error_string(err);
}

/******************************************************************************\
 *  UART interface implementation                                             *
\******************************************************************************/
bool uart_open(uart_t * out, size_t baud_rate) {
    out->open = true;
}

uart_error_t uart_write_byte(uart_t * channel, uint8_t byte) {
    if (!channel->open) {
        return UART_CHANNEL_CLOSED;
    }
    channel->output.push_back(byte);

    return UART_NO_ERROR;
}

uart_error_t uart_read_byte(uart_t * channel, uint8_t * output) {
    if (!channel->open) {
        return UART_CHANNEL_CLOSED;
    }
    if (channel->input.size() == 0) {
        return UART_SIGNAL_FAULT;
    }
    *output = channel->input.back();
    channel->input.pop_back();

    return UART_NO_ERROR;
}

/******************************************************************************\
 *  HasWrittenBytes implementation                                             *
\******************************************************************************/
HasWrittenBytes::HasWrittenBytes(const std::initializer_list<uint8_t> data) :
    _bytes(data) {}

HasWrittenBytes::HasWrittenBytes(const std::vector<uint8_t> data) :
    _bytes(data) {}

bool HasWrittenBytes::match(const uart_t & e) const {
    if (e.output.size() < _bytes.size()) {
        return false;
    }
    auto output_it = e.output.begin() + (e.output.size() - _bytes.size());
    auto bytes_it = _bytes.begin();

    auto end_it = e.output.end();
    while(output_it != end_it) {
        if (*output_it != *bytes_it) {
            return false;
        }
        ++output_it;
        ++bytes_it;
    }

    return true;
}

std::string HasWrittenBytes::describe() const {
    std::stringstream ss;
    ss << "has output the byte stream";
    ss.setf(std::ios::hex);
    for (const auto & i : _bytes) {
        ss << " " << std::setw(2) << std::setfill('0') << (int)i;
    }
    return ss.str();
}

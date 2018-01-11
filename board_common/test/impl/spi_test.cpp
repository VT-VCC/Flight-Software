#include "spi_test.hpp"

void spi_open(spi_t * channel) {
  channel->_impl = new spi_impl();
  channel->_impl->open = true;
}

void spi_close(spi_t * out) {
  delete out->_impl;
  out->_impl = nullptr;  
}

spi_error_t spi_transfer_bytes(spi_t * channel, uint8_t * send_bytes, uint8_t * receive_bytes, size_t length) {
  if (!channel->_impl || !channel->_impl->open) {
    return SPI_CHANNEL_CLOSED;
  }
  //This is just an example SPI device where the returned value is
  //always one greater than the given value. This doesn't actually
  //make sense since SPI is sychrnonous, but whatever. It's an
  //exmaple.
  memcpy(receive_bytes, send_bytes, length);

  channel->_impl->mosi_bytes.insert(channel->_impl->mosi_bytes.end(), &send_bytes[0], &send_bytes[length]);
  channel->_impl->miso_bytes.insert(channel->_impl->miso_bytes.end(), &receive_bytes[0], &receive_bytes[length]);

  return SPI_NO_ERROR;
}

HasMasterInSlaveOutBytes::HasMasterInSlaveOutBytes(const std::initializer_list<uint8_t> data) :
    _bytes(data) {}

HasMasterInSlaveOutBytes::HasMasterInSlaveOutBytes(const std::vector<uint8_t> data) :
    _bytes(data) {}

bool HasMasterInSlaveOutBytes::match(const spi_t & e) const {
    if (!e._impl) {
        return false;
    }
    if (e._impl->miso_bytes.size() < _bytes.size()) {
        return false;
    }
    std::vector<uint8_t> & output = e._impl->miso_bytes;
    auto output_it = output.begin() + (output.size() - _bytes.size());
    auto bytes_it = _bytes.begin();

    auto end_it = output.end();
    while(output_it != end_it) {
        if (*output_it != *bytes_it) {
            return false;
        }
        ++output_it;
        ++bytes_it;
    }

    return true;
}

std::string HasMasterInSlaveOutBytes::describe() const {
    std::stringstream ss;
    ss << "MISO has the byte stream";
    ss.setf(std::ios::hex);
    for (const auto & i : _bytes) {
        ss << " " << std::setw(2) << std::setfill('0') << (int)i;
    }
    return ss.str();
}

////

HasMasterOutSlaveInBytes::HasMasterOutSlaveInBytes(const std::initializer_list<uint8_t> data) :
    _bytes(data) {}

HasMasterOutSlaveInBytes::HasMasterOutSlaveInBytes(const std::vector<uint8_t> data) :
    _bytes(data) {}

bool HasMasterOutSlaveInBytes::match(const spi_t & e) const {
    if (!e._impl) {
        return false;
    }
    if (e._impl->mosi_bytes.size() < _bytes.size()) {
        return false;
    }
    std::vector<uint8_t> & output = e._impl->mosi_bytes;
    auto output_it = output.begin() + (output.size() - _bytes.size());
    auto bytes_it = _bytes.begin();

    auto end_it = output.end();
    while(output_it != end_it) {
        if (*output_it != *bytes_it) {
            return false;
        }
        ++output_it;
        ++bytes_it;
    }

    return true;
}

std::string HasMasterOutSlaveInBytes::describe() const {
    std::stringstream ss;
    ss << "MOSI has the byte stream";
    ss.setf(std::ios::hex);
    for (const auto & i : _bytes) {
        ss << " " << std::setw(2) << std::setfill('0') << (int)i;
    }
    return ss.str();
}

std::ostream & operator<<(std::ostream & o, const spi_t & t) {
     if (t._impl == NULL) {
       o << "{spi: closed}";
       return o;
     }
     o << "{spi: output { miso: ";
   
    auto f = o.flags();
    o.setf(std::ios::hex);
    for (const auto & i : t._impl->miso_bytes) {
        o << " " << std::setw(2) << std::setfill('0') << (int)i;
    }
    o.flags(f);

    o << " } { mosi: ";
    
    f = o.flags();
    o.setf(std::ios::hex);
    for (const auto & i : t._impl->mosi_bytes) {
        o << " " << std::setw(2) << std::setfill('0') << (int)i;
    }
    o.flags(f);

    o << " } }";
    return o;
}

std::ostream & operator<<(std::ostream & o, const spi_error_t & err) {
    return o << spi_error_string(err);
}

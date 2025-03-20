#include "ip/port.hpp"

#include <arpa/inet.h>

namespace core::ip {

port::port(std::uint16_t port) noexcept : port_(port) {}

port::port(std::uint16_t port, network_byte_order_t) noexcept
    : port_(ntohs(port)) {}

std::uint16_t port::get_bytes() const noexcept { return port_; }

std::uint16_t port::get_bytes(network_byte_order_t) const noexcept {
  return htons(port_);
}

}  // namespace core::ip

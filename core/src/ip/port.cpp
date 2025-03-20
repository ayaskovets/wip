#include "ip/port.hpp"

namespace core::ip {

port::port(std::uint16_t port) noexcept : port_(port) {}

std::uint16_t port::as_bytes() const noexcept { return port_; }

}  // namespace core::ip

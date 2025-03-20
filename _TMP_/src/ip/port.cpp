#include "ip/port.hpp"

namespace _TMP_::ip {

port::port(std::uint16_t port) noexcept : port_(port) {}

std::uint16_t port::as_bytes() const noexcept { return port_; }

}  // namespace _TMP_::ip

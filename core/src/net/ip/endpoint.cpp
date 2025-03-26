#include "net/ip/endpoint.hpp"

namespace core::net::ip {

endpoint::endpoint(ip::address address, net::ip::port port) noexcept
    : address_(std::move(address)), port_(std::move(port)) {}

const net::ip::address& endpoint::get_address() const noexcept {
  return address_;
}

ip::port endpoint::get_port() const noexcept { return port_; }

std::string endpoint::to_string() const {
  switch (address_.get_version()) {
    case net::ip::version::kIPv4:
      return std::format("{}:{}", address_, port_);
    case net::ip::version::kIPv6:
      return std::format("[{}]:{}", address_, port_);
  }
}

}  // namespace core::net::ip

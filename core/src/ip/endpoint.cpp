#include "ip/endpoint.hpp"

namespace core::ip {

endpoint::endpoint(ip::address address, ip::port port) noexcept
    : address_(std::move(address)), port_(std::move(port)) {}

const ip::address& endpoint::get_address() const noexcept { return address_; }

ip::port endpoint::get_port() const noexcept { return port_; }

std::string endpoint::to_string() const {
  switch (address_.get_version()) {
    case ip::version::kIpV4:
      return std::format("{}:{}", address_, port_);
    case ip::version::kIpV6:
      return std::format("[{}]:{}", address_, port_);
  }
}

}  // namespace core::ip

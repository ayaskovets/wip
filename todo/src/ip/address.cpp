#include "ip/address.hpp"

#include <arpa/inet.h>

#include <format>

namespace todo::ip {

address::address(std::string address) {
  address_.resize(sizeof(in_addr));
  if (inet_pton(AF_INET, address.c_str(), address_.data())) {
    return;
  }

  address_.resize(sizeof(in6_addr));
  if (inet_pton(AF_INET6, address.c_str(), address_.data())) {
    return;
  }

  throw std::invalid_argument(std::format("invalid address {}", address));
}

address::address(std::vector<std::uint8_t> address)
    : address_(std::move(address)) {
  switch (version()) {
    case version::kIpV4:
    case version::kIpV6:
      break;
  }
}

version address::version() const {
  switch (address_.size()) {
    case sizeof(in_addr):
      return version::kIpV4;
    case sizeof(in6_addr):
      return version::kIpV6;
    default:
      throw std::runtime_error(
          std::format("invalid address size {}", address_.size()));
  }
}

std::string address::as_string() const {
  std::string string;

  int family;
  switch (version()) {
    case version::kIpV4:
      family = AF_INET;
      string.resize(INET_ADDRSTRLEN);
      break;
    case version::kIpV6:
      family = AF_INET6;
      string.resize(INET6_ADDRSTRLEN);
      break;
  }

  if (inet_ntop(family, address_.data(), string.data(), string.size()))
      [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize address: {}", strerror(errno)));
}

const std::vector<std::uint8_t>& address::as_bytes() const { return address_; }

}  // namespace todo::ip

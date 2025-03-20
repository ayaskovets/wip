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

address::address(std::vector<std::uint8_t> address) {
  switch (address.size()) {
    case sizeof(in_addr):
    case sizeof(in6_addr):
      address_ = std::move(address);
      break;
    default:
      throw std::invalid_argument(
          std::format("invalid address size {}", address.size()));
  }
}

std::string address::as_string() const {
  std::string string;
  decltype(AF_INET) family;
  switch (address_.size()) {
    case sizeof(in_addr):
      family = AF_INET;
      string.resize(INET_ADDRSTRLEN);
      break;
    case sizeof(in6_addr):
      family = AF_INET6;
      string.resize(INET6_ADDRSTRLEN);
      break;
    default:
      throw std::runtime_error(
          std::format("unexpected address size: {}", address_.size()));
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

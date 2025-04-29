#include "net/inet6/ip.hpp"

#include <arpa/inet.h>

#include <cerrno>
#include <format>

namespace core::net::inet6 {

namespace {

constexpr int kInetPtonSuccess = 1;
constexpr auto kInetNtopError = nullptr;

}  // namespace

const ip& ip::kLoopback() noexcept {
  static const ip ip("::1");
  return ip;
}

const ip& ip::kAny() noexcept {
  static const ip ip("::");
  return ip;
}

const ip& ip::kNonRoutable() noexcept {
  static const ip ip("2001:db8::");
  return ip;
}

ip::ip(std::span<const std::uint8_t, 16> bytes, network_byte_order_t) noexcept {
  std::memcpy(data_.data(), bytes.data(), bytes.size());
}

ip::ip(std::string_view string) {
  if (string.size() > INET6_ADDRSTRLEN) {
    throw std::invalid_argument(std::format("invalid ip: {}", string));
  }
  if (::inet_pton(AF_INET6, string.data(), &data_) != kInetPtonSuccess) {
    throw std::invalid_argument(
        std::format("invalid ip {}: {}", string, std::strerror(errno)));
  }
}

const std::array<std::uint8_t, 16>& ip::get_bytes(
    network_byte_order_t) const noexcept {
  return data_;
}

std::string ip::to_string() const {
  std::string string;
  string.resize(INET6_ADDRSTRLEN);

  if (::inet_ntop(AF_INET6, &data_, string.data(), string.size()) !=
      kInetNtopError) [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize ip: {}", std::strerror(errno)));
}

}  // namespace core::net::inet6

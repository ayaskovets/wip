#include "net/ip/address.hpp"

#include <arpa/inet.h>

#include <cerrno>
#include <format>

namespace core::net::ip {

namespace {

constexpr std::size_t kIPv4Bytes = 4;
constexpr std::size_t kIPv6Bytes = 16;

}  // namespace

const address& address::kLocalhost(ip::version version) noexcept {
  switch (version) {
    case net::ip::version::kIPv4: {
      static address address("127.0.0.1");
      return address;
    }
    case net::ip::version::kIPv6: {
      static address address("::1");
      return address;
    }
  }
}

const address& address::kAny(ip::version version) noexcept {
  switch (version) {
    case net::ip::version::kIPv4: {
      static address address("0.0.0.0");
      return address;
    }
    case net::ip::version::kIPv6: {
      static address address("::");
      return address;
    }
  }
}

const address& address::kBroadcast(/* net::ip::version::kIPv4 */) noexcept {
  static address address("255.255.255.255");
  return address;
}

const address& address::kNonRoutable(ip::version version) noexcept {
  switch (version) {
    case net::ip::version::kIPv4: {
      static address address("192.0.2.0");
      return address;
    }
    case net::ip::version::kIPv6: {
      static address address("2001:db8::");
      return address;
    }
  }
}

address::address(std::span<const std::uint8_t> data) {
  switch (data.size()) {
    case kIPv4Bytes:
      std::memcpy(data_.begin(), data.data(), data.size());
      version_ = net::ip::version::kIPv4;
      break;
    case kIPv6Bytes:
      std::memcpy(data_.begin(), data.data(), data.size());
      version_ = net::ip::version::kIPv6;
      break;
    [[unlikely]] default:
      throw std::invalid_argument(
          std::format("invalid address size {}", data.size()));
  }
}

address::address(std::string_view string_view) {
  if (string_view.size() <= INET_ADDRSTRLEN &&
      ::inet_pton(AF_INET, string_view.data(), data_.begin())) {
    version_ = net::ip::version::kIPv4;
  } else if (string_view.size() <= INET6_ADDRSTRLEN &&
             ::inet_pton(AF_INET6, string_view.data(), data_.begin())) {
    version_ = net::ip::version::kIPv6;
  } else {
    throw std::invalid_argument(std::format("invalid address {}", string_view));
  }
}

bool address::operator==(const address& that) const noexcept {
  switch (version_) {
    case net::ip::version::kIPv4:
      return version_ == that.version_ &&
             std::equal(data_.begin(), std::next(data_.begin(), kIPv4Bytes),
                        that.data_.begin(),
                        std::next(that.data_.begin(), kIPv4Bytes));
    case net::ip::version::kIPv6:
      return version_ == that.version_ && data_ == that.data_;
  }
}

bool address::operator!=(const address& that) const noexcept {
  return !operator==(that);
}

std::span<const std::uint8_t> address::get_bytes() const noexcept {
  switch (version_) {
    case net::ip::version::kIPv4:
      return std::span<const std::uint8_t>(data_.begin(), kIPv4Bytes);
    case net::ip::version::kIPv6:
      return std::span<const std::uint8_t>(data_.begin(), kIPv6Bytes);
  }
}

ip::version address::get_version() const noexcept { return version_; }

std::string address::to_string() const {
  std::string string;

  int family;
  switch (version_) {
    case net::ip::version::kIPv4:
      family = AF_INET;
      string.resize(INET_ADDRSTRLEN);
      break;
    case net::ip::version::kIPv6:
      family = AF_INET6;
      string.resize(INET6_ADDRSTRLEN);
      break;
  }

  if (::inet_ntop(family, &data_[0], string.data(), string.size())) [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize address: {}", std::strerror(errno)));
}

}  // namespace core::net::ip

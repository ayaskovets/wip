#include "ip/address.hpp"

#include <arpa/inet.h>

#include <cerrno>
#include <format>

namespace _TMP_::ip {

namespace {

constexpr std::size_t kIpV4Bytes = 4;
constexpr std::size_t kIpV6Bytes = 16;

}  // namespace

address::address(const std::uint8_t* data, std::size_t size) {
  switch (size) {
    case kIpV4Bytes:
      std::memcpy(data_.begin(), data, size);
      version_ = version::kIpV4;
      break;
    case kIpV6Bytes:
      std::memcpy(data_.begin(), data, size);
      version_ = version::kIpV6;
      break;
    default:
      throw std::invalid_argument(std::format("invalid address size {}", size));
  }
}

address::address(std::string_view string_view) {
  if (string_view.size() <= INET_ADDRSTRLEN &&
      inet_pton(AF_INET, string_view.data(), data_.begin())) {
    version_ = version::kIpV4;
  } else if (string_view.size() <= INET6_ADDRSTRLEN &&
             inet_pton(AF_INET6, string_view.data(), data_.begin())) {
    version_ = version::kIpV6;
  } else {
    throw std::invalid_argument(std::format("invalid address {}", string_view));
  }
}

bool address::operator==(const address& that) const noexcept {
  switch (version_) {
    case version::kIpV4:
      return this->version_ == that.version_ &&
             std::equal(this->data_.begin(),
                        std::next(this->data_.begin(), kIpV4Bytes),
                        that.data_.begin(),
                        std::next(that.data_.begin(), kIpV4Bytes));
    case version::kIpV6:
      return this->version_ == that.version_ && this->data_ == that.data_;
  }
}

version address::get_version() const { return version_; }

std::span<const std::uint8_t> address::get_bytes() const {
  switch (version_) {
    case version::kIpV4:
      return std::span<const std::uint8_t>(data_.begin(), kIpV4Bytes);
    case version::kIpV6:
      return std::span<const std::uint8_t>(data_.begin(), kIpV6Bytes);
  }
}

std::string address::as_string() const {
  std::string string;

  int family;
  switch (version_) {
    case version::kIpV4:
      family = AF_INET;
      string.resize(INET_ADDRSTRLEN);
      break;
    case version::kIpV6:
      family = AF_INET6;
      string.resize(INET6_ADDRSTRLEN);
      break;
  }

  if (inet_ntop(family, data_.begin(), string.data(), string.size()))
      [[likely]] {
    string.resize(string.find('\0'));
    return string;
  }

  throw std::runtime_error(
      std::format("could not serialize address: {}", strerror(errno)));
}

}  // namespace _TMP_::ip

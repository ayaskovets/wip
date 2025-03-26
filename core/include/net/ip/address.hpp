#pragma once

#include <array>
#include <format>
#include <ranges>
#include <span>
#include <string>

#include "net/ip/version.hpp"

namespace core::net::ip {

class address final {
 public:
  static const address& kLocalhost(net::ip::version version) noexcept;
  static const address& kNonRoutable(net::ip::version version) noexcept;
  static const address& kAny(net::ip::version version) noexcept;
  static const address& kBroadcast(/* net::ip::version::kIPv4 */) noexcept;

 public:
  explicit address(std::span<const std::uint8_t> bytes);
  explicit address(std::string_view string_view);

 public:
  bool operator==(const address& that) const noexcept;
  bool operator!=(const address& that) const noexcept;

 public:
  std::span<const std::uint8_t> get_bytes() const noexcept;
  net::ip::version get_version() const noexcept;

 public:
  std::string to_string() const;

 private:
  std::array<std::uint8_t, 16> data_;
  net::ip::version version_;
};

}  // namespace core::net::ip

template <>
struct std::formatter<core::net::ip::address> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::ip::address& address,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", address.to_string());
  }
};

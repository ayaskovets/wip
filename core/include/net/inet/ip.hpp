#pragma once

#include <format>
#include <string>

namespace core::net::inet {

class ip final {
 public:
  static const ip& kLocalhost() noexcept;
  static const ip& kNonRoutable() noexcept;
  static const ip& kAny() noexcept;
  static const ip& kBroadcast() noexcept;

 public:
  struct network_byte_order_t final {};

 public:
  explicit ip(std::uint32_t bytes) noexcept;
  explicit ip(std::uint32_t bytes, network_byte_order_t) noexcept;
  explicit ip(std::string_view string);

 public:
  constexpr bool operator==(const ip& other) const = default;
  constexpr bool operator!=(const ip& other) const = default;

 public:
  std::uint32_t get_bytes() const noexcept;
  std::uint32_t get_bytes(network_byte_order_t) const noexcept;

 public:
  std::string to_string() const;

 private:
  std::uint32_t data_;
};

}  // namespace core::net::inet

template <>
struct std::formatter<core::net::inet::ip> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::inet::ip& ip,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", ip.to_string());
  }
};

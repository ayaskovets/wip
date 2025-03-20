#pragma once

#include <cstdint>
#include <format>

namespace core::ip {

class port final {
 public:
  struct network_byte_order_t {};
  constinit static inline network_byte_order_t network_byte_order{};

 public:
  explicit port(std::uint16_t port) noexcept;
  port(std::uint16_t port, network_byte_order_t) noexcept;

 public:
  constexpr bool operator==(const port&) const noexcept = default;
  constexpr bool operator!=(const port&) const noexcept = default;

 public:
  std::uint16_t get_bytes() const noexcept;
  std::uint16_t get_bytes(network_byte_order_t) const noexcept;

 private:
  std::uint16_t port_;
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::port> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::ip::port& port, FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", port.get_bytes());
  }
};

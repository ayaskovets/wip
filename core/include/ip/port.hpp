#pragma once

#include <cstdint>
#include <format>

namespace core::ip {

class port final {
 public:
  explicit port(std::uint16_t port) noexcept;

 public:
  std::uint16_t get_bytes() const noexcept;

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

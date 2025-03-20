#pragma once

#include <cstdint>
#include <format>

namespace todo::ip {

enum class protocol : std::uint8_t {
  kTcp = 0,
  kUdp = 1,
};

}  // namespace todo::ip

template <>
struct std::formatter<todo::ip::protocol> {
  template <class format_context>
  [[nodiscard]] constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  [[nodiscard]] constexpr auto format(const todo::ip::protocol& protocol,
                                      format_context& ctx) const {
    switch (protocol) {
      case todo::ip::protocol::kTcp:
        return std::format_to(ctx.out(), "TCP");
      case todo::ip::protocol::kUdp:
        return std::format_to(ctx.out(), "UDP");
    }
  }
};

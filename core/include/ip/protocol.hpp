#pragma once

#include <cstdint>
#include <format>

namespace core::ip {

enum class protocol : std::uint8_t {
  kTcp,
  kUdp,
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::protocol> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) const {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const core::ip::protocol& protocol,
                        format_context& ctx) const {
    switch (protocol) {
      case core::ip::protocol::kTcp:
        return std::format_to(ctx.out(), "TCP");
      case core::ip::protocol::kUdp:
        return std::format_to(ctx.out(), "UDP");
    }
  }
};

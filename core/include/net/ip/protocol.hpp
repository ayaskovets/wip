#pragma once

#include <cstdint>
#include <format>

namespace core::net::ip {

enum class protocol : std::uint8_t {
  kTcp,
  kUdp,
};

}  // namespace core::net::ip

template <>
struct std::formatter<core::net::ip::protocol> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::ip::protocol& protocol,
                        FormatContext& ctx) const {
    switch (protocol) {
      case core::net::ip::protocol::kTcp:
        return std::format_to(ctx.out(), "TCP");
      case core::net::ip::protocol::kUdp:
        return std::format_to(ctx.out(), "UDP");
    }
  }
};

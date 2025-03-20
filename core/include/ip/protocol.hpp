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
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::ip::protocol& protocol,
                        FormatContext& ctx) const {
    switch (protocol) {
      case core::ip::protocol::kTcp:
        return std::format_to(ctx.out(), "TCP");
      case core::ip::protocol::kUdp:
        return std::format_to(ctx.out(), "UDP");
    }
  }
};

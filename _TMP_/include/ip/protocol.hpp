#pragma once

#include <cstdint>
#include <format>

namespace _TMP_::ip {

enum class protocol : std::uint8_t {
  kTcp = 0,
  kUdp = 1,
};

}  // namespace _TMP_::ip

template <>
struct std::formatter<_TMP_::ip::protocol> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const _TMP_::ip::protocol& protocol,
                        format_context& ctx) const {
    switch (protocol) {
      case _TMP_::ip::protocol::kTcp:
        return std::format_to(ctx.out(), "TCP");
      case _TMP_::ip::protocol::kUdp:
        return std::format_to(ctx.out(), "UDP");
    }
  }
};

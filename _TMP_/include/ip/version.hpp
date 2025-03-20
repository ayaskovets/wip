#pragma once

#include <cstdint>
#include <format>

namespace _TMP_::ip {

enum class version : std::uint8_t {
  kIpV4,
  kIpV6,
};

}  // namespace _TMP_::ip

template <>
struct std::formatter<_TMP_::ip::version> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const _TMP_::ip::version& version,
                        format_context& ctx) const {
    switch (version) {
      case _TMP_::ip::version::kIpV4:
        return std::format_to(ctx.out(), "IPv4");
      case _TMP_::ip::version::kIpV6:
        return std::format_to(ctx.out(), "IPv6");
    }
  }
};

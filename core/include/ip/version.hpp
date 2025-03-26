#pragma once

#include <cstdint>
#include <format>

namespace core::ip {

enum class version : std::uint8_t {
  kIPv4,
  kIPv6,
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::version> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::ip::version& version,
                        FormatContext& ctx) const {
    switch (version) {
      case core::ip::version::kIPv4:
        return std::format_to(ctx.out(), "IPv4");
      case core::ip::version::kIPv6:
        return std::format_to(ctx.out(), "IPv6");
    }
  }
};

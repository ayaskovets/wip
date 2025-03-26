#pragma once

#include <cstdint>
#include <format>

namespace core::net::ip {

enum class version : std::uint8_t {
  kIPv4,
  kIPv6,
};

}  // namespace core::net::ip

template <>
struct std::formatter<core::net::ip::version> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::ip::version& version,
                        FormatContext& ctx) const {
    switch (version) {
      case core::net::ip::version::kIPv4:
        return std::format_to(ctx.out(), "IPv4");
      case core::net::ip::version::kIPv6:
        return std::format_to(ctx.out(), "IPv6");
    }
  }
};

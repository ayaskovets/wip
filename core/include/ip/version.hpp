#pragma once

#include <cstdint>
#include <format>

namespace core::ip {

enum class version : std::uint8_t {
  kIpV4,
  kIpV6,
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::version> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const core::ip::version& version,
                        format_context& ctx) const {
    switch (version) {
      case core::ip::version::kIpV4:
        return std::format_to(ctx.out(), "IPv4");
      case core::ip::version::kIpV6:
        return std::format_to(ctx.out(), "IPv6");
    }
  }
};

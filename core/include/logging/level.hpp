#pragma once

#include <cstdint>
#include <format>

namespace core::logging {

enum class level : std::uint8_t {
  kDebug = 0,
  kInfo = 1,
  kWarn = 2,
  kError = 3,
};

}  // namespace core::logging

template <>
struct std::formatter<core::logging::level> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) const {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const core::logging::level& level,
                        format_context& ctx) const {
    switch (level) {
      case core::logging::level::kDebug:
        return std::format_to(ctx.out(), "DEBUG");
      case core::logging::level::kInfo:
        return std::format_to(ctx.out(), "INFO");
      case core::logging::level::kWarn:
        return std::format_to(ctx.out(), "WARN");
      case core::logging::level::kError:
        return std::format_to(ctx.out(), "ERROR");
    }
  }
};

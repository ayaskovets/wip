#pragma once

#include <cstdint>
#include <format>

namespace todo::logging {

enum class level : std::uint8_t {
  kDebug = 0,
  kInfo = 1,
  kWarn = 2,
  kError = 3,
};

}  // namespace todo::logging

template <>
struct std::formatter<todo::logging::level> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const todo::logging::level& level,
                        format_context& ctx) const {
    switch (level) {
      case todo::logging::level::kDebug:
        return std::format_to(ctx.out(), "DEBUG");
      case todo::logging::level::kInfo:
        return std::format_to(ctx.out(), "INFO");
      case todo::logging::level::kWarn:
        return std::format_to(ctx.out(), "WARN");
      case todo::logging::level::kError:
        return std::format_to(ctx.out(), "ERROR");
    }
  }
};

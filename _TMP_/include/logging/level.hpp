#pragma once

#include <cstdint>
#include <format>

namespace _TMP_::logging {

enum class level : std::uint8_t {
  kDebug = 0,
  kInfo = 1,
  kWarn = 2,
  kError = 3,
};

}  // namespace _TMP_::logging

template <>
struct std::formatter<_TMP_::logging::level> {
  template <class format_context>
  constexpr auto parse(format_context& ctx) {
    return ctx.begin();
  }

  template <class format_context>
  constexpr auto format(const _TMP_::logging::level& level,
                        format_context& ctx) const {
    switch (level) {
      case _TMP_::logging::level::kDebug:
        return std::format_to(ctx.out(), "DEBUG");
      case _TMP_::logging::level::kInfo:
        return std::format_to(ctx.out(), "INFO");
      case _TMP_::logging::level::kWarn:
        return std::format_to(ctx.out(), "WARN");
      case _TMP_::logging::level::kError:
        return std::format_to(ctx.out(), "ERROR");
    }
  }
};

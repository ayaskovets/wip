#pragma once

#include "logging/level.hpp"
#include "logging/logger.hpp"
#include "logging/renderer.hpp"
#include "logging/writer.hpp"

namespace _TMP_::logging {

template <typename... Args>
constexpr decltype(auto) debug(std::format_string<Args...> fmt,
                               Args&&... args) {
  return logger::kDefault()(level::kDebug, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr decltype(auto) info(std::format_string<Args...> fmt, Args&&... args) {
  return logger::kDefault()(level::kInfo, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr decltype(auto) warning(std::format_string<Args...> fmt,
                                 Args&&... args) {
  return logger::kDefault()(level::kWarn, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr decltype(auto) error(std::format_string<Args...> fmt,
                               Args&&... args) {
  return logger::kDefault()(level::kError, fmt, std::forward<Args>(args)...);
}

}  // namespace _TMP_::logging
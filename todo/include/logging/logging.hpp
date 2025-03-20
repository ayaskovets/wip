#pragma once

#include "logging/level.hpp"
#include "logging/logger.hpp"
#include "logging/renderer.hpp"
#include "logging/writer.hpp"

namespace todo::logging {

template <typename... Args>
constexpr void debug(std::format_string<Args...> fmt, Args&&... args) {
  logger::kDefault()(level::kDebug, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void info(std::format_string<Args...> fmt, Args&&... args) {
  logger::kDefault()(level::kInfo, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void warning(std::format_string<Args...> fmt, Args&&... args) {
  logger::kDefault()(level::kWarn, fmt, std::forward<Args>(args)...);
}

template <typename... Args>
constexpr void error(std::format_string<Args...> fmt, Args&&... args) {
  logger::kDefault()(level::kError, fmt, std::forward<Args>(args)...);
}

}  // namespace todo::logging
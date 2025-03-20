#pragma once

#include <format>

#include "logging/level.hpp"
#include "logging/renderer.hpp"
#include "logging/writer.hpp"

namespace _TMP_::logging {

class logger {
 public:
  static const logger& kColoredLeveledTimestampedStderr();
  static const logger& kDefault();

 public:
  logger(level level, renderer renderer, writer writer) noexcept
      : level_(level),
        renderer_(std::move(renderer)),
        writer_(std::move(writer)) {}

 public:
  template <typename... Args>
  constexpr void operator()(level level, std::format_string<Args...> fmt,
                            Args&&... args) const {
    if (level_ <= level) [[likely]] {
      writer_(level,
              renderer_(level, fmt.get(),
                        std::make_format_args(std::forward<Args>(args)...)));
    }
  }

 protected:
  level level_;
  renderer renderer_;
  writer writer_;
};

}  // namespace _TMP_::logging
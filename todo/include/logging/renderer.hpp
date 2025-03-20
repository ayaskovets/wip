#pragma once

#include <format>
#include <string_view>

#include "logging/level.hpp"

namespace todo::logging {

class renderer final {
 private:
  using render_t = std::string (*)(level level, std::string_view fmt,
                                   std::format_args args);

 public:
  static const renderer& kNoop();
  static const renderer& kColoredLeveled();
  static const renderer& kColoredLeveledTimestamped();
  static const renderer& kDefault();

 public:
  constexpr explicit renderer(render_t render) noexcept
      : render_(std::move(render)) {}

 public:
  constexpr std::string operator()(level level, std::string_view fmt,
                                   std::format_args args) const {
    return render_(level, fmt, std::move(args));
  }

 private:
  render_t render_;
};

}  // namespace todo::logging

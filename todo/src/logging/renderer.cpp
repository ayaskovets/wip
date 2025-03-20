#include "logging/renderer.hpp"

#include "datetime/iso8601.hpp"

namespace todo::logging {

namespace {

constexpr auto kSetColor(level level) noexcept -> auto {
  constexpr auto kGreenColor = "\x1B[32m";
  constexpr auto kRedColor = "\x1B[31m";
  constexpr auto kYellowColor = "\x1B[33m";
  switch (level) {
    case level::kDebug:
      return "";
    case level::kInfo:
      return kGreenColor;
    case level::kWarn:
      return kYellowColor;
    case level::kError:
      return kRedColor;
  }
}
constexpr auto kResetColor = "\033[0m";

}  // namespace

const renderer& renderer::kNoop() {
  static renderer renderer([]([[maybe_unused]] level level,
                              std::string_view fmt,
                              std::format_args args) -> std::string {
    return std::vformat(fmt, std::move(args));
  });
  return renderer;
}

const renderer& renderer::kColoredLeveled() {
  static renderer renderer([](level level, std::string_view fmt,
                              std::format_args args) -> std::string {
    return std::vformat(
        std::format("{}{}{}\t{}\n", kSetColor(level), level, kResetColor, fmt),
        std::move(args));
  });
  return renderer;
}

const renderer& renderer::kColoredLeveledTimestamped() {
  static renderer renderer([](level level, std::string_view fmt,
                              std::format_args args) -> std::string {
    return std::vformat(
        std::format("{}\t{}{}{}\t{}\n",
                    datetime::as_iso8601(std::chrono::system_clock::now()),
                    kSetColor(level), level, kResetColor, fmt),
        std::move(args));
  });
  return renderer;
}

const renderer& renderer::kDefault() { return kColoredLeveledTimestamped(); }

}  // namespace todo::logging

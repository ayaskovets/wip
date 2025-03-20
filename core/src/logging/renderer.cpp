#include "logging/renderer.hpp"

#include "datetime/iso8601.hpp"

namespace core::logging {

namespace {

constexpr auto kSetColor(logging::level level) noexcept {
  constexpr auto kGreenColor = "\x1B[32m";
  constexpr auto kRedColor = "\x1B[31m";
  constexpr auto kYellowColor = "\x1B[33m";
  switch (level) {
    case logging::level::kDebug:
      return "";
    case logging::level::kInfo:
      return kGreenColor;
    case logging::level::kWarn:
      return kYellowColor;
    case logging::level::kError:
      return kRedColor;
  }
}
constexpr auto kResetColor = "\033[0m";

}  // namespace

const renderer& renderer::kNoop() {
  static renderer renderer(
      []([[maybe_unused]] logging::level level, std::string_view fmt,
         std::format_args args) { return std::vformat(fmt, std::move(args)); });
  return renderer;
}

const renderer& renderer::kColoredLeveled() {
  static renderer renderer([](logging::level level, std::string_view fmt,
                              std::format_args args) {
    return std::vformat(
        std::format("{}{}{}\t{}\n", kSetColor(level), level, kResetColor, fmt),
        std::move(args));
  });
  return renderer;
}

const renderer& renderer::kColoredLeveledTimestamped() {
  static renderer renderer(
      [](logging::level level, std::string_view fmt, std::format_args args) {
        return std::vformat(
            std::format("{}\t{}{}{}\t{}\n",
                        datetime::to_iso8601(std::chrono::system_clock::now()),
                        kSetColor(level), level, kResetColor, fmt),
            std::move(args));
      });
  return renderer;
}

const renderer& renderer::kDefault() { return kColoredLeveledTimestamped(); }

}  // namespace core::logging

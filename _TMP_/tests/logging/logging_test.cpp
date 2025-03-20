#include "logging/logging.hpp"

#include <gtest/gtest.h>

namespace tests::logging {

TEST(_TMP__logging, size) {
  static_assert(sizeof(_TMP_::logging::logger) == 48);
  static_assert(alignof(_TMP_::logging::logger) == 8);
}

TEST(_TMP__logging, writer) {
  std::string buffer;

  const auto level = _TMP_::logging::level::kInfo;
  const auto renderer = _TMP_::logging::renderer::kNoop();
  const auto writer = _TMP_::logging::writer(
      [&buffer]([[maybe_unused]] _TMP_::logging::level level,
                std::string_view message) { buffer = message; });
  const auto logger = _TMP_::logging::logger(level, renderer, writer);

  logger(level, "{}", 42);
  EXPECT_EQ(buffer, "42");
}

TEST(_TMP__logging, skip) {
  std::size_t logs_count = 0;

  const auto level = _TMP_::logging::level::kWarn;
  const auto renderer = _TMP_::logging::renderer::kNoop();
  const auto writer = _TMP_::logging::writer(
      [&logs_count]([[maybe_unused]] _TMP_::logging::level level,
                    [[maybe_unused]] std::string_view message) {
        logs_count += 1;
      });
  const auto logger = _TMP_::logging::logger(level, renderer, writer);

  logger(_TMP_::logging::level::kDebug, "message");
  EXPECT_EQ(logs_count, 0);

  logger(_TMP_::logging::level::kInfo, "message");
  EXPECT_EQ(logs_count, 0);

  logger(_TMP_::logging::level::kWarn, "message");
  EXPECT_EQ(logs_count, 1);

  logger(_TMP_::logging::level::kError, "message");
  EXPECT_EQ(logs_count, 2);
}

TEST(_TMP__logging, renderer) {
  std::string buffer;

  const auto level = _TMP_::logging::level::kInfo;
  const auto renderer =
      _TMP_::logging::renderer([]([[maybe_unused]] _TMP_::logging::level level,
                                  std::string_view fmt, std::format_args args) {
        return std::vformat(fmt, std::move(args)) +
               std::vformat(fmt, std::move(args));
      });
  const auto writer = _TMP_::logging::writer(
      [&buffer]([[maybe_unused]] _TMP_::logging::level level,
                std::string_view message) { buffer = message; });
  const auto logger = _TMP_::logging::logger(level, renderer, writer);

  logger(_TMP_::logging::level::kInfo, "{}", 42);
  EXPECT_EQ(buffer, "4242");
}

}  // namespace tests::logging

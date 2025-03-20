#include "logging/logging.hpp"

#include <gtest/gtest.h>

namespace tests::logging {

namespace logging = _TMP_::logging;

TEST(_TMP__logging, writer) {
  std::string buffer;
  const logging::logger logger(
      logging::level::kInfo, logging::renderer::kNoop(),
      logging::writer(
          [&buffer]([[maybe_unused]] logging::level level,
                    std::string_view message) { buffer = message; }));

  logger(logging::level::kInfo, "{}", 42);
  EXPECT_EQ(buffer, "42");
}

TEST(_TMP__logging, skip) {
  std::string buffer;
  const logging::logger logger(
      logging::level::kWarn, logging::renderer::kNoop(),
      logging::writer([&buffer]([[maybe_unused]] logging::level level,
                                [[maybe_unused]] std::string_view message) {
        buffer.resize(buffer.size() + 1);
      }));

  logger(logging::level::kDebug, "message");
  EXPECT_TRUE(buffer.empty());

  logger(logging::level::kInfo, "message");
  EXPECT_TRUE(buffer.empty());

  logger(logging::level::kWarn, "message");
  EXPECT_EQ(buffer.size(), 1);

  logger(logging::level::kError, "message");
  EXPECT_EQ(buffer.size(), 2);
}

TEST(_TMP__logging, renderer) {
  std::string buffer;
  const logging::logger logger(
      logging::level::kInfo,
      logging::renderer([]([[maybe_unused]] logging::level level,
                           std::string_view fmt,
                           std::format_args args) -> std::string {
        return std::vformat(fmt, std::move(args)) +
               std::vformat(fmt, std::move(args));
      }),
      logging::writer(
          [&buffer]([[maybe_unused]] logging::level level,
                    std::string_view message) { buffer = message; }));

  logger(logging::level::kInfo, "{}", 42);
  EXPECT_EQ(buffer, "4242");
}

}  // namespace tests::logging

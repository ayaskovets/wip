#include "logging/logging.hpp"

#include <gtest/gtest.h>

namespace tests::logging {

TEST(_TMP__logging, writer) {
  std::string buffer;
  const _TMP_::logging::logger logger(
      _TMP_::logging::level::kInfo, _TMP_::logging::renderer::kNoop(),
      _TMP_::logging::writer(
          [&buffer]([[maybe_unused]] _TMP_::logging::level level,
                    std::string_view message) { buffer = message; }));

  logger(_TMP_::logging::level::kInfo, "{}", 42);
  EXPECT_EQ(buffer, "42");
}

TEST(_TMP__logging, skip) {
  std::string buffer;
  const _TMP_::logging::logger logger(
      _TMP_::logging::level::kWarn, _TMP_::logging::renderer::kNoop(),
      _TMP_::logging::writer(
          [&buffer]([[maybe_unused]] _TMP_::logging::level level,
                    [[maybe_unused]] std::string_view message) {
            buffer.resize(buffer.size() + 1);
          }));

  logger(_TMP_::logging::level::kDebug, "message");
  EXPECT_TRUE(buffer.empty());

  logger(_TMP_::logging::level::kInfo, "message");
  EXPECT_TRUE(buffer.empty());

  logger(_TMP_::logging::level::kWarn, "message");
  EXPECT_EQ(buffer.size(), 1);

  logger(_TMP_::logging::level::kError, "message");
  EXPECT_EQ(buffer.size(), 2);
}

TEST(_TMP__logging, renderer) {
  std::string buffer;
  const _TMP_::logging::logger logger(
      _TMP_::logging::level::kInfo,
      _TMP_::logging::renderer([]([[maybe_unused]] _TMP_::logging::level level,
                                  std::string_view fmt,
                                  std::format_args args) -> std::string {
        return std::vformat(fmt, std::move(args)) +
               std::vformat(fmt, std::move(args));
      }),
      _TMP_::logging::writer(
          [&buffer]([[maybe_unused]] _TMP_::logging::level level,
                    std::string_view message) { buffer = message; }));

  logger(_TMP_::logging::level::kInfo, "{}", 42);
  EXPECT_EQ(buffer, "4242");
}

}  // namespace tests::logging

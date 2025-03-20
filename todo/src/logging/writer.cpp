#include "logging/writer.hpp"

#include <cstdio>
#include <format>

namespace todo::logging {

const writer& writer::kStdout() {
  static writer writer(
      []([[maybe_unused]] level level, std::string_view message) {
        if (fputs(message.data(), stdout) == EOF) [[unlikely]] {
          throw std::runtime_error(std::format("fputs(stdout) failed: {}",
                                               strerror(ferror(stdout))));
        }
      });
  return writer;
}

const writer& writer::kStderr() {
  static writer writer(
      []([[maybe_unused]] level level, std::string_view message) {
        if (fputs(message.data(), stderr) == EOF) [[unlikely]] {
          throw std::runtime_error(std::format("fputs(stderr) failed: {}",
                                               strerror(ferror(stderr))));
        }
      });
  return writer;
}

const writer& writer::kNoop() {
  static writer writer([]([[maybe_unused]] level level,
                          [[maybe_unused]] std::string_view message) {});
  return writer;
}

const writer& writer::kDefault() { return kStderr(); }

}  // namespace todo::logging

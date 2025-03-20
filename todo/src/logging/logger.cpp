#include "logging/logger.hpp"

namespace todo::logging {

const logger &logger::kColoredLeveledTimestampedStderr() {
  static logger logger(level::kInfo, renderer::kDefault(), writer::kDefault());
  return logger;
}

const logger &logger::kDefault() { return kColoredLeveledTimestampedStderr(); }

}  // namespace todo::logging

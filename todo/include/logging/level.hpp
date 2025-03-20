#pragma once

#include <cstdint>

namespace todo::logging {

enum class level : std::uint8_t {
  kDebug = 0,
  kInfo = 1,
  kWarn = 2,
  kError = 3,
};

}  // namespace todo::logging

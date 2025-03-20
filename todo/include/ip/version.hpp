#pragma once

#include <cstdint>

namespace todo::ip {

enum class version : std::uint8_t {
  kIpV4,
  kIpV6,
};

}  // namespace todo::ip

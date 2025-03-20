#pragma once

#include <concepts>

namespace core::utils {

template <std::integral T>
constexpr bool is_power_of_two(T value) {
  return value > 0 && ((value & (value - 1)) == 0);
}

}  // namespace core::utils

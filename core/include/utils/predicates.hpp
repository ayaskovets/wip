#pragma once

#include <concepts>

namespace core::utils {

template <std::integral T>
constexpr bool is_power_of_two(T value) {
  return value > 0 && ((value & (value - static_cast<T>(1))) == 0);
}

template <std::integral T>
constexpr bool is_even(T value) {
  return !(value & 1);
}

}  // namespace core::utils

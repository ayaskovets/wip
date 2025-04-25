#pragma once

#include <cstddef>

namespace core::utils {

constexpr const std::size_t kCacheLineSize = 64;

template <std::unsigned_integral T>
constexpr const T kDynamicCapacity = 0;

}  // namespace core::utils

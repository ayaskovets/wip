#pragma once

#include <cstddef>
#include <memory>
#include <new>

namespace core::utils {

template <typename T, std::size_t Alignment>
struct aligned final {
  alignas(Alignment) T value;
};

}  // namespace core::utils

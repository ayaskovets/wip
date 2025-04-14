#pragma once

#include <concepts>
#include <utility>

#include "utils/mixins.hpp"

namespace core::utils {

template <std::invocable T>
class scope_exit final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(
      noexcept(std::declval<T>()()),
      "scope_exit invocable must be noexcept not to throw in the destructor");

 public:
  constexpr explicit scope_exit(T&& finally)
      : finally_(std::forward<T>(finally)) {}
  constexpr ~scope_exit() noexcept { finally_(); }

 private:
  T finally_;
};

}  // namespace core::utils

#pragma once

#include <concepts>
#include <utility>

#include "utils/mixins.hpp"

namespace core::utils {

template <std::invocable T>
  requires(noexcept(std::declval<T>()()))
class scope_exit final : utils::non_copyable, utils::non_movable {
 public:
  constexpr explicit scope_exit(T&& finally)
      : finally_(std::forward<T>(finally)) {}
  constexpr ~scope_exit() noexcept { finally_(); }

 private:
  T finally_;
};

}  // namespace core::utils

#pragma once

#include <concepts>
#include <utility>

namespace core::utils {

template <std::invocable T>
class scope_exit final {
 private:
  static_assert(noexcept(std::declval<T>()()));

 public:
  constexpr explicit scope_exit(T&& finally)
      : finally_(std::forward<T>(finally)) {}
  constexpr ~scope_exit() noexcept { finally_(); }

 private:
  T finally_;
};

}  // namespace core::utils

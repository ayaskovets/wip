#pragma once

#include <concepts>
#include <utility>

namespace core::utils {

template <std::invocable T>
class scope_exit final {
 public:
  constexpr explicit scope_exit(T&& finally)
      : finally_(std::forward<T>(finally)) {}
  constexpr ~scope_exit() {
    try {
      finally_();
    } catch (...) {
    }
  }

 private:
  T finally_;
};

}  // namespace core::utils

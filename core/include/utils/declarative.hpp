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

#define _ANONYMOUS_VARIABLE_CAT(name, line) name##line
#define _ANONYMOUS_VARIABLE(name, line) _ANONYMOUS_VARIABLE_CAT(name, line)

#define SCOPE_EXIT(f) \
  const ::core::utils::scope_exit _ANONYMOUS_VARIABLE(scope_exit_, __LINE__)(f);

}  // namespace core::utils

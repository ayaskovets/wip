#pragma once

namespace todo::utils {

template <typename T>
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
  const todo::utils::scope_exit _ANONYMOUS_VARIABLE(scope_exit_, __LINE__)(f);

}  // namespace todo::utils

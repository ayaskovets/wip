#pragma once

#include <utility>

namespace core::utils {

template <typename T, bool IsRuntime, T CompileTimeValue = T()>
class conditionally_runtime;

template <typename T, T CompileTimeValue>
class conditionally_runtime<T, true, CompileTimeValue> final {
 public:
  constexpr conditionally_runtime() = delete;
  constexpr conditionally_runtime(T value) : value_(std::move(value)) {}

 public:
  constexpr T& operator*() noexcept { return value_; }
  constexpr const T& operator*() const noexcept { return value_; }

 private:
  T value_;
};

template <typename T, T CompileTimeValue>
class conditionally_runtime<T, false, CompileTimeValue> final {
 public:
  constexpr conditionally_runtime() noexcept = default;

 public:
  constexpr const T& operator*() const noexcept { return value_; }

 private:
  static inline const constinit T value_ = CompileTimeValue;
};

}  // namespace core::utils

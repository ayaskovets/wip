#pragma once

namespace core::utils {

struct non_copyable {
  constexpr non_copyable() noexcept = default;
  constexpr non_copyable(const non_copyable&) = delete;
  constexpr non_copyable& operator=(const non_copyable&) = delete;
};

struct non_movable {
  constexpr non_movable() noexcept = default;
  constexpr non_movable(non_copyable&&) = delete;
  constexpr non_movable& operator=(non_movable&&) = delete;
};

}  // namespace core::utils

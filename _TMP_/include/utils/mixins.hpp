#pragma once

namespace _TMP_::utils {

struct non_copyable {
  non_copyable() = default;
  non_copyable(const non_copyable&) = delete;
  non_copyable& operator=(const non_copyable&) = delete;
};

struct non_movable {
  non_movable() = default;
  non_movable(non_copyable&&) = delete;
  non_movable& operator=(non_movable&&) = delete;
};

}  // namespace _TMP_::utils

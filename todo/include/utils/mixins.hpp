#pragma once

namespace todo::utils {

struct uncopyable {
  uncopyable() = default;
  uncopyable(const uncopyable&) = delete;
  uncopyable& operator=(const uncopyable&) = delete;
};

struct unmovable {
  unmovable() = default;
  unmovable(uncopyable&&) = delete;
  unmovable& operator=(unmovable&&) = delete;
};

}  // namespace todo::utils

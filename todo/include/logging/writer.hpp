#pragma once

#include <functional>
#include <string_view>

#include "logging/level.hpp"

namespace todo::logging {

class writer final {
 private:
  using write_t = std::function<void(level level, std::string_view message)>;

 public:
  static const writer& kStdout();
  static const writer& kStderr();
  static const writer& kNoop();
  static const writer& kDefault();

 public:
  explicit writer(write_t write) : write_(write) {}

 public:
  constexpr void operator()(level level, std::string_view message) const {
    write_(level, message);
  }

 private:
  write_t write_;
};

}  // namespace todo::logging

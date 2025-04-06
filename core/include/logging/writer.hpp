#pragma once

#include <functional>
#include <string_view>

#include "logging/level.hpp"

namespace core::logging {

class writer final {
 public:
  static const writer& kStdout();
  static const writer& kStderr();
  static const writer& kNoop();
  static const writer& kDefault();

 private:
  using write_t =
      std::function<void(logging::level level, std::string_view message)>;

 public:
  explicit writer(write_t write) : write_(write) {}

 public:
  constexpr void operator()(logging::level level,
                            std::string_view message) const {
    write_(level, message);
  }

 private:
  write_t write_;
};

}  // namespace core::logging

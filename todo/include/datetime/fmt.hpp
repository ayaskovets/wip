#pragma once

#include <chrono>

namespace todo::datetime {

[[nodiscard]] std::string to_iso8601(
    std::chrono::system_clock::time_point time_point);
[[nodiscard]] std::chrono::system_clock::time_point from_utc_iso8601(
    std::string_view timestamp);

}  // namespace todo::datetime

#pragma once

#include <chrono>

namespace todo::datetime {

std::string to_iso8601(std::chrono::system_clock::time_point time_point);

}  // namespace todo::datetime

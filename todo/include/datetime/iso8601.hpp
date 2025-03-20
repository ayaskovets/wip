#pragma once

#include <chrono>

namespace todo::datetime {

std::string as_iso8601(std::chrono::system_clock::time_point time_point);

}  // namespace todo::datetime

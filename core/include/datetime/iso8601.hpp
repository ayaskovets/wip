#pragma once

#include <chrono>
#include <string>

namespace core::datetime {

std::string as_iso8601(std::chrono::system_clock::time_point time_point);

}  // namespace core::datetime

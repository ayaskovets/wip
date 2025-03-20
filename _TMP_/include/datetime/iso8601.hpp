#pragma once

#include <chrono>

namespace _TMP_::datetime {

std::string as_iso8601(std::chrono::system_clock::time_point time_point);

}  // namespace _TMP_::datetime

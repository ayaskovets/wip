#include "datetime/iso8601.hpp"

namespace todo::datetime {

std::string as_iso8601(std::chrono::system_clock::time_point time_point) {
  constexpr auto kIso8601Fmt = "{}-{:0>2}-{:0>2}T{:0>2}:{:0>2}:{:0>2}.{:0>6}Z";

  const std::time_t time = std::chrono::system_clock::to_time_t(time_point);
  std::tm tm;
  /* std:: */ gmtime_r(&time, &tm);
  const auto tm_msec =
      time_point.time_since_epoch() -
      std::chrono::system_clock::from_time_t(time).time_since_epoch();

  return std::format(kIso8601Fmt, 1900 + tm.tm_year, 1 + tm.tm_mon, tm.tm_mday,
                     tm.tm_hour, tm.tm_min, tm.tm_sec, tm_msec.count());
}

}  // namespace todo::datetime

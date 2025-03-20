#include "datetime/fmt.hpp"

namespace todo::datetime {

std::string to_iso8601(std::chrono::system_clock::time_point time_point) {
  constexpr auto kIso8601Fmt = "{}-{:0>2}-{:0>2}T{:0>2}:{:0>2}:{:0>2}.{:0>6}Z";
  constexpr auto kTmBaseYear = 1900;
  constexpr auto kJanuaryNumber = 1;

  const std::time_t time = std::chrono::system_clock::to_time_t(time_point);
  std::tm tm;
  /* std:: */ gmtime_r(&time, &tm);
  const auto tm_msec =
      time_point.time_since_epoch() -
      std::chrono::system_clock::from_time_t(time).time_since_epoch();

  return std::format(kIso8601Fmt, kTmBaseYear + tm.tm_year,
                     kJanuaryNumber + tm.tm_mon, tm.tm_mday, tm.tm_hour,
                     tm.tm_min, tm.tm_sec, tm_msec.count());
}

}  // namespace todo::datetime

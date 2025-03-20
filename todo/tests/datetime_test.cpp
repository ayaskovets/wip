#include <gtest/gtest.h>

#include "datetime/iso8601.hpp"

namespace tests::logging {

namespace datetime = todo::datetime;

TEST(todo, as_iso8601) {
  using namespace std::chrono;
  const system_clock::time_point time_point =
      sys_days{January / 9 / 2014} + 12h + 35min + 34s + 125ms + 322us;
  const auto timestamp = "2014-01-09T12:35:34.125322Z";

  EXPECT_EQ(datetime::as_iso8601(time_point), timestamp);
}

}  // namespace tests::logging

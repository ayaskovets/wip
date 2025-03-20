#include <gtest/gtest.h>

#include "datetime/serialization.hpp"

namespace tests::logging {

namespace logging = todo::datetime;

TEST(todo, iso8601) {
  using namespace std::chrono;
  const system_clock::time_point time_point =
      sys_days{January / 9 / 2014} + 12h + 35min + 34s + 125ms + 322us;

  EXPECT_EQ(todo::datetime::to_iso8601(time_point),
            "2014-01-09T12:35:34.125322Z");
}

}  // namespace tests::logging

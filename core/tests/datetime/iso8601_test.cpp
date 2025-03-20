#include "datetime/iso8601.hpp"

#include <gtest/gtest.h>

namespace tests::datetime {

class core_datetime
    : public ::testing::TestWithParam<
          std::tuple<std::chrono::system_clock::time_point, std::string_view>> {
};

TEST_P(core_datetime, as_iso8601) {
  const auto& [time_point, string] = GetParam();
  EXPECT_EQ(core::datetime::as_iso8601(time_point), string);
}

using namespace std::chrono_literals;
INSTANTIATE_TEST_SUITE_P(
    core_datetime, core_datetime,
    ::testing::Values(
        std::make_tuple(std::chrono::sys_days{std::chrono::January / 9 / 2014} +
                            12h + 35min + 34s + 125ms + 322us,
                        "2014-01-09T12:35:34.125322Z"),
        std::make_tuple(std::chrono::sys_days{std::chrono::January / 9 / 1300} +
                            12h + 35min + 34s + 125ms + 322us,
                        "1300-01-09T12:35:34.125322Z"),
        std::make_tuple(std::chrono::sys_days{std::chrono::January / 9 / 3235} +
                            12h + 35min + 34s + 125ms + 322us,
                        "3235-01-09T12:35:34.125322Z")));

}  // namespace tests::datetime

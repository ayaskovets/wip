#include "ip/address.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(_TMP__ip, address_size) {
  static_assert(sizeof(_TMP_::ip::address) == 24);
  static_assert(alignof(_TMP_::ip::address) == 8);
}

class _TMP__ip
    : public ::testing::TestWithParam<std::tuple<
          std::vector<std::uint8_t>, std::string, _TMP_::ip::version>> {};

TEST_P(_TMP__ip, address_construction) {
  const auto [bytes, string, version] = GetParam();

  const auto from_bytes = _TMP_::ip::address(bytes);
  const auto from_string = _TMP_::ip::address(string);

  EXPECT_EQ(from_bytes.as_bytes(), bytes);
  EXPECT_EQ(from_bytes.as_string(), string);
  EXPECT_EQ(from_bytes.version(), version);

  EXPECT_EQ(from_bytes.as_bytes(), from_string.as_bytes());
  EXPECT_EQ(from_bytes.as_string(), from_string.as_string());
  EXPECT_EQ(from_bytes.version(), from_string.version());
  EXPECT_EQ(from_bytes, from_string);
}

INSTANTIATE_TEST_SUITE_P(
    _TMP__ip, _TMP__ip,
    ::testing::Values(
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255},
                        "255.255.255.255", _TMP_::ip::version::kIpV4),
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255},
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        _TMP_::ip::version::kIpV6)));

}  // namespace tests::ip

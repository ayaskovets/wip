#include "ip/address.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(_TMP__ip, address_size) {
  static_assert(sizeof(_TMP_::ip::address) == 17);
  static_assert(alignof(_TMP_::ip::address) == 1);
}

class _TMP__ip
    : public ::testing::TestWithParam<std::tuple<
          std::vector<std::uint8_t>, std::string, _TMP_::ip::version>> {};

TEST_P(_TMP__ip, address_construction) {
  const auto& [bytes, string, version] = GetParam();

  const _TMP_::ip::address from_bytes(bytes);
  const _TMP_::ip::address from_string(string);

  EXPECT_TRUE(from_bytes == from_string);
  EXPECT_FALSE(from_bytes != from_string);
  EXPECT_TRUE(std::equal(bytes.begin(), bytes.end(),
                         from_bytes.get_bytes().begin(),
                         from_bytes.get_bytes().end()));
  EXPECT_EQ(from_bytes.as_string(), string);
  EXPECT_EQ(from_bytes.get_version(), version);

  EXPECT_TRUE(std::equal(
      from_bytes.get_bytes().begin(), from_bytes.get_bytes().end(),
      from_string.get_bytes().begin(), from_string.get_bytes().end()));
  EXPECT_EQ(from_bytes.as_string(), from_string.as_string());
  EXPECT_EQ(from_bytes.get_version(), from_string.get_version());
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

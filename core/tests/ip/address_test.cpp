#include "ip/address.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip, address_size) {
  static_assert(sizeof(core::ip::address) == 17);
  static_assert(alignof(core::ip::address) == 1);
}

class ip : public ::testing::TestWithParam<std::tuple<
               std::vector<std::uint8_t>, std::string, core::ip::version>> {};

TEST_P(ip, address_construction) {
  const auto& [bytes, string, version] = GetParam();

  const core::ip::address from_bytes(bytes);
  const core::ip::address from_string(string);

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
    ip, ip,
    ::testing::Values(
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255},
                        "255.255.255.255", core::ip::version::kIpV4),
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255},
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        core::ip::version::kIpV6)));

}  // namespace tests::ip

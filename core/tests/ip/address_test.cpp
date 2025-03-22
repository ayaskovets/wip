#include "ip/address.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_address, size) {
  static_assert(sizeof(core::ip::address) == 17);
  static_assert(alignof(core::ip::address) == 1);
}

TEST(ip_address, localhost) {
  EXPECT_EQ(core::ip::address("127.0.0.1"),
            core::ip::address::kLocalhost(core::ip::version::kIpV4));
  EXPECT_EQ(core::ip::address("::1"),
            core::ip::address::kLocalhost(core::ip::version::kIpV6));
}

class ip_address
    : public ::testing::TestWithParam<std::tuple<
          std::vector<std::uint8_t>, std::string, core::ip::version>> {};

TEST_P(ip_address, construction) {
  const auto& [bytes, string, version] = GetParam();

  const core::ip::address from_bytes(bytes);
  const core::ip::address from_string(string);

  EXPECT_TRUE(from_bytes == from_string);
  EXPECT_FALSE(from_bytes != from_string);
  EXPECT_TRUE(std::equal(bytes.begin(), bytes.end(),
                         from_bytes.get_bytes().begin(),
                         from_bytes.get_bytes().end()));
  EXPECT_EQ(from_bytes.to_string(), string);
  EXPECT_EQ(from_bytes.get_version(), version);

  EXPECT_TRUE(std::equal(
      from_bytes.get_bytes().begin(), from_bytes.get_bytes().end(),
      from_string.get_bytes().begin(), from_string.get_bytes().end()));
  EXPECT_EQ(from_bytes.to_string(), from_string.to_string());
  EXPECT_EQ(from_bytes.get_version(), from_string.get_version());
  EXPECT_EQ(from_bytes, from_string);
}

INSTANTIATE_TEST_SUITE_P(
    ip_address, ip_address,
    ::testing::Values(
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255},
                        "255.255.255.255", core::ip::version::kIpV4),
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255},
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        core::ip::version::kIpV6)));

}  // namespace tests::ip

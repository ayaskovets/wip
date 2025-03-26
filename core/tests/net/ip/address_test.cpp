#include "net/ip/address.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_address, size) {
  static_assert(sizeof(core::net::ip::address) == 17);
  static_assert(alignof(core::net::ip::address) == 1);
}

TEST(ip_address, localhost) {
  EXPECT_EQ(core::net::ip::address("127.0.0.1"),
            core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4));
  EXPECT_EQ(core::net::ip::address("::1"),
            core::net::ip::address::kLocalhost(core::net::ip::version::kIPv6));
}

TEST(ip_address, any) {
  EXPECT_EQ(core::net::ip::address("0.0.0.0"),
            core::net::ip::address::kAny(core::net::ip::version::kIPv4));
  EXPECT_EQ(core::net::ip::address("::"),
            core::net::ip::address::kAny(core::net::ip::version::kIPv6));
}

TEST(ip_address, broadcast) {
  EXPECT_EQ(core::net::ip::address("255.255.255.255"),
            core::net::ip::address::kBroadcast());
}

class ip_address
    : public ::testing::TestWithParam<std::tuple<
          std::vector<std::uint8_t>, std::string, core::net::ip::version>> {};

TEST_P(ip_address, construction) {
  const auto& [bytes, string, version] = GetParam();

  const core::net::ip::address from_bytes(bytes);
  const core::net::ip::address from_string(string);

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
                        "255.255.255.255", core::net::ip::version::kIPv4),
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255},
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        core::net::ip::version::kIPv6)));

}  // namespace tests::ip

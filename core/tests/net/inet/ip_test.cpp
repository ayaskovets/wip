#include "net/inet/ip.hpp"

#include <gtest/gtest.h>

namespace tests::net::inet {

TEST(net_inet_ip, size) {
  static_assert(sizeof(core::net::inet::ip) == 4);
  static_assert(alignof(core::net::inet::ip) == 4);
}

TEST(net_inet_ip, localhost) {
  EXPECT_EQ(core::net::inet::ip("127.0.0.1"),
            core::net::inet::ip::kLocalhost());
}

TEST(net_inet_ip, any) {
  EXPECT_EQ(core::net::inet::ip("0.0.0.0"), core::net::inet::ip::kAny());
}

TEST(net_inet_ip, broadcast) {
  EXPECT_EQ(core::net::inet::ip("255.255.255.255"),
            core::net::inet::ip::kBroadcast());
}

TEST(net_inet_ip, construction) {
  const std::uint32_t bytes{0xFFFFFF00};
  const std::string string("255.255.255.0");

  const core::net::inet::ip from_bytes(bytes);
  const core::net::inet::ip from_string(string);

  EXPECT_EQ(from_bytes, from_string);
  EXPECT_EQ(bytes, from_bytes.get_bytes());
  EXPECT_EQ(from_bytes.to_string(), string);
  EXPECT_EQ(from_bytes.get_bytes(), from_string.get_bytes());
  EXPECT_EQ(from_bytes.to_string(), from_string.to_string());
}

TEST(net_inet_ip, network_order) {
  const std::uint32_t bytes{0xFFFFFF00};

  const core::net::inet::ip host_byte_order(bytes);
  const core::net::inet::ip network_byte_order(
      bytes, core::net::inet::ip::network_byte_order_t{});

  EXPECT_EQ(host_byte_order.get_bytes(), bytes);
  EXPECT_NE(
      host_byte_order.get_bytes(core::net::inet::ip::network_byte_order_t{}),
      bytes);
  EXPECT_NE(network_byte_order.get_bytes(), bytes);
  EXPECT_EQ(
      network_byte_order.get_bytes(core::net::inet::ip::network_byte_order_t{}),
      bytes);
}

TEST(net_inet_ip, invalid) {
  EXPECT_ANY_THROW(core::net::inet::ip(""));
  EXPECT_ANY_THROW(core::net::inet::ip("256.0.0.0"));
  EXPECT_ANY_THROW(core::net::inet::ip("11132123123abc"));
}

TEST(net_inet_ip, to_string) {
  EXPECT_EQ(core::net::inet::ip("255.255.255.0").to_string(), "255.255.255.0");
  EXPECT_EQ(core::net::inet::ip("255.255.255.0").to_string(), "255.255.255.0");
}

}  // namespace tests::net::inet

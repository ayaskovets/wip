#include "net/ip/dns.hpp"

#include <gtest/gtest.h>

namespace tests::net::ip {

TEST(net_ip_dns, resolve_localhost_tcp_ip6) {
  const auto results =
      core::net::ip::resolve("localhost", core::net::ip::protocol::kTcp,
                             core::net::ip::version::kIPv6);
  EXPECT_EQ(results.size(), 1);

  const auto& endpoint = results.at(0);
  EXPECT_EQ(endpoint.get_address(), core::net::ip::address("::1"));
}

TEST(net_ip_dns, resolve_localhost_udp_ip4) {
  const auto results =
      core::net::ip::resolve("localhost", core::net::ip::protocol::kUdp,
                             core::net::ip::version::kIPv4);
  EXPECT_EQ(results.size(), 1);

  const auto& endpoint = results.at(0);
  EXPECT_EQ(endpoint.get_address(), core::net::ip::address("127.0.0.1"));
}

}  // namespace tests::net::ip

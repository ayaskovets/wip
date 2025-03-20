#include "ip/dns.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip, resolve_localhost_tcp_ip6) {
  const auto results = core::ip::resolve("localhost", core::ip::protocol::kTcp,
                                         core::ip::version::kIpV6);
  EXPECT_EQ(results.size(), 1);

  const auto& endpoint = results.at(0);
  EXPECT_EQ(endpoint.get_address(), core::ip::address("::1"));
}

TEST(ip, resolve_localhost_udp_ip4) {
  const auto results = core::ip::resolve("localhost", core::ip::protocol::kUdp,
                                         core::ip::version::kIpV4);
  EXPECT_EQ(results.size(), 1);

  const auto& endpoint = results.at(0);
  EXPECT_EQ(endpoint.get_address(), core::ip::address("127.0.0.1"));
}

}  // namespace tests::ip

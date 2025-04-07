#include "net/dns/resolve.hpp"

#include <gtest/gtest.h>

namespace tests::net::dns {

TEST(net_dns_resolve, localhost_udp_ip4) {
  const auto results =
      core::net::dns::resolve("localhost", core::net::sockets::family::kInet,
                              core::net::sockets::protocol::kUdp);
  EXPECT_EQ(results.size(), 1);

  const auto& sockaddr = results.at(0);
  EXPECT_EQ(std::get<core::net::inet::sockaddr>(sockaddr).get_ip(),
            core::net::inet::ip("127.0.0.1"));
}

TEST(net_dns_resolve, localhost_tcp_ip6) {
  const auto results =
      core::net::dns::resolve("localhost", core::net::sockets::family::kInet6,
                              core::net::sockets::protocol::kTcp);
  EXPECT_EQ(results.size(), 1);

  const auto& sockaddr = results.at(0);
  EXPECT_EQ(std::get<core::net::inet6::sockaddr>(sockaddr).get_ip(),
            core::net::inet6::ip("::1"));
}

TEST(net_dns_resolve, localhost_unspec) {
  const auto results = core::net::dns::resolve(
      "localhost", core::net::sockets::family::kUnspecified,
      core::net::sockets::protocol::kUnspecified);
  EXPECT_EQ(results.size(), 4);
}

}  // namespace tests::net::dns

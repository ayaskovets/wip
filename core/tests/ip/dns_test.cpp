#include "ip/dns.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(core_ip, resolve_localhost_tcp_ip6) {
  const auto addresses = core::ip::resolve(
      "localhost", core::ip::protocol::kTcp, core::ip::version::kIpV6);
  EXPECT_EQ(addresses,
            (std::vector<core::ip::address>{core::ip::address("::1")}));
}

TEST(core_ip, resolve_localhost_udp_ip4) {
  const auto addresses = core::ip::resolve(
      "localhost", core::ip::protocol::kUdp, core::ip::version::kIpV4);
  EXPECT_EQ(addresses,
            (std::vector<core::ip::address>{core::ip::address("127.0.0.1")}));
}

}  // namespace tests::ip

#include "ip/dns.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(_TMP__ip, resolve_localhost_tcp_ip6) {
  const auto addresses = _TMP_::ip::resolve(
      "localhost", _TMP_::ip::protocol::kTcp, _TMP_::ip::version::kIpV6);
  EXPECT_EQ(addresses,
            (std::vector<_TMP_::ip::address>{_TMP_::ip::address("::1")}));
}

TEST(_TMP__ip, resolve_localhost_udp_ip4) {
  const auto addresses = _TMP_::ip::resolve(
      "localhost", _TMP_::ip::protocol::kUdp, _TMP_::ip::version::kIpV4);
  EXPECT_EQ(addresses,
            (std::vector<_TMP_::ip::address>{_TMP_::ip::address("127.0.0.1")}));
}

}  // namespace tests::ip

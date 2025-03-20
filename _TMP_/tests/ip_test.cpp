#include <gtest/gtest.h>

#include "ip/address.hpp"
#include "ip/dns.hpp"

namespace tests::logging {

namespace ip = _TMP_::ip;

TEST(_TMP_, ip4_address_constructor) {
  const std::vector<std::uint8_t> bytes{255, 255, 255, 255};
  const std::string string("255.255.255.255");

  const auto address1 = ip::address(bytes);
  const auto address2 = ip::address(string);

  EXPECT_EQ(address1.as_bytes(), bytes);
  EXPECT_EQ(address1.as_string(), string);
  EXPECT_EQ(address1.version(), ip::version::kIpV4);

  EXPECT_EQ(address1.as_bytes(), address2.as_bytes());
  EXPECT_EQ(address1.as_string(), address2.as_string());
  EXPECT_EQ(address1.version(), address2.version());
  EXPECT_EQ(address1, address2);
}

TEST(_TMP_, ip6_address_constructor) {
  const std::vector<std::uint8_t> bytes{255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255};
  const std::string string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");

  const auto address1 = ip::address(bytes);
  const auto address2 = ip::address(string);

  EXPECT_EQ(address1, address2);
  EXPECT_EQ(address1.as_bytes(), bytes);
  EXPECT_EQ(address1.as_string(), string);
  EXPECT_EQ(address1.version(), ip::version::kIpV6);

  EXPECT_EQ(address1.as_bytes(), address2.as_bytes());
  EXPECT_EQ(address1.as_string(), address2.as_string());
  EXPECT_EQ(address1.version(), address2.version());
  EXPECT_EQ(address1, address2);
}

TEST(_TMP_, resolve_localhost) {
  const auto addresses = ip::resolve("localhost", ip::protocol::kTcp);

  EXPECT_EQ(addresses, (std::vector<ip::address>{ip::address("::1"),
                                                 ip::address("127.0.0.1")}));
}

}  // namespace tests::logging

#include "net/ip/port.hpp"

#include <gtest/gtest.h>

namespace tests::net::ip {

TEST(ip_port, size) {
  static_assert(sizeof(core::net::ip::port) == 2);
  static_assert(alignof(core::net::ip::port) == 2);
}

TEST(ip_port, construction) {
  EXPECT_NE(core::net::ip::port(1234),
            core::net::ip::port(1234, core::net::ip::port::network_byte_order));
  EXPECT_EQ(
      core::net::ip::port(65535),
      core::net::ip::port(65535, core::net::ip::port::network_byte_order));
  EXPECT_EQ(core::net::ip::port(0),
            core::net::ip::port(0, core::net::ip::port::network_byte_order));
}

}  // namespace tests::net::ip

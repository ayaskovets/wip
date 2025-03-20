#include "ip/port.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip, port_size) {
  static_assert(sizeof(core::ip::port) == 2);
  static_assert(alignof(core::ip::port) == 2);
}

TEST(ip, port_construction) {
  EXPECT_NE(core::ip::port(1234),
            core::ip::port(1234, core::ip::port::network_byte_order));
  EXPECT_EQ(core::ip::port(65535),
            core::ip::port(65535, core::ip::port::network_byte_order));
  EXPECT_EQ(core::ip::port(0),
            core::ip::port(0, core::ip::port::network_byte_order));
}

}  // namespace tests::ip

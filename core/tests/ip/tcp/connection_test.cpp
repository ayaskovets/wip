#include "ip/tcp/connection.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_tcp, connection_size) {
  static_assert(sizeof(core::ip::tcp::connection) == 4);
  static_assert(alignof(core::ip::tcp::connection) == 4);
}

// TODO: tests

}  // namespace tests::ip

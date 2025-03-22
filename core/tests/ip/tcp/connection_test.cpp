#include "ip/tcp/connection.hpp"

#include <gtest/gtest.h>

namespace tests::ip::tcp {

TEST(ip_tcp_connection, size) {
  static_assert(sizeof(core::ip::tcp::connection) == 4);
  static_assert(alignof(core::ip::tcp::connection) == 4);
}

// TODO: tests

}  // namespace tests::ip::tcp

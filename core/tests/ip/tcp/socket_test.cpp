#include "ip/tcp/socket.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_tcp, socket_size) {
  static_assert(sizeof(core::ip::tcp::socket) == 4);
  static_assert(alignof(core::ip::tcp::socket) == 4);
}

// TODO: tests

}  // namespace tests::ip

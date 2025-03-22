#include "ip/tcp/socket.hpp"

#include <gtest/gtest.h>

namespace tests::ip::tcp {

TEST(ip_tcp_socket, size) {
  static_assert(sizeof(core::ip::tcp::socket) == 4);
  static_assert(alignof(core::ip::tcp::socket) == 4);
}

}  // namespace tests::ip::tcp

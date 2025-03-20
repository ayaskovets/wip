#include "ip/socket.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip, socket_size) {
  static_assert(sizeof(core::ip::socket) == 4);
  static_assert(alignof(core::ip::socket) == 4);
}

// TODO: socket tests

}  // namespace tests::ip

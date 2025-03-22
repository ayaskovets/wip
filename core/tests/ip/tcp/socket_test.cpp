#include "ip/tcp/socket.hpp"

#include <gtest/gtest.h>

namespace tests::ip::tcp {

TEST(ip_tcp_socket, size) {
  static_assert(sizeof(core::ip::tcp::socket) == 4);
  static_assert(alignof(core::ip::tcp::socket) == 4);
}

TEST(ip_tcp_socket, try_accept) {
  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint endpoint(core::ip::address::kLocalhost(version),
                                      core::ip::port(9997));
    core::ip::tcp::socket socket(version);
    socket.bind(endpoint);
    socket.set_flag(core::ip::socket::flag::kNonblocking, true);
    EXPECT_ANY_THROW(socket.accept());
    socket.listen(1);
    EXPECT_FALSE(socket.try_accept().has_value());
  }
}

// TODO: send/recv

}  // namespace tests::ip::tcp

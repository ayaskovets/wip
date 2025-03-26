#include "net/ip/socket.hpp"

#include <gtest/gtest.h>

namespace tests::net::ip {

TEST(ip_socket, size) {
  static_assert(sizeof(core::net::ip::socket) == 4);
  static_assert(alignof(core::net::ip::socket) == 4);
}

TEST(ip_socket, flags) {
  for (const auto protocol :
       {core::net::ip::protocol::kTcp, core::net::ip::protocol::kUdp}) {
    for (const auto version :
         {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
      core::net::ip::socket socket(protocol, version);
      for (const auto flag : {core::net::ip::socket::flag::kNonblocking,
                              core::net::ip::socket::flag::kReuseaddr,
                              core::net::ip::socket::flag::kReuseport,
                              core::net::ip::socket::flag::kKeepalive}) {
        EXPECT_FALSE(socket.get_flag(flag));
        socket.set_flag(flag, true);
        EXPECT_TRUE(socket.get_flag(flag));
        socket.set_flag(flag, false);
        EXPECT_FALSE(socket.get_flag(flag));
      }
    }
  }
}

TEST(ip_socket, bind) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    const core::net::ip::endpoint endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9999));
    for (const auto protocol :
         {core::net::ip::protocol::kTcp, core::net::ip::protocol::kUdp}) {
      core::net::ip::socket socket(protocol, version);
      socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::net::ip::socket::flag::kReuseport, true);
      socket.bind(endpoint);
      EXPECT_ANY_THROW(socket.bind(endpoint));
    }
  }
}

TEST(ip_socket, connect) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    const core::net::ip::endpoint endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9998));
    for (const auto protocol :
         {core::net::ip::protocol::kTcp, core::net::ip::protocol::kUdp}) {
      core::net::ip::socket socket(protocol, version);
      socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::net::ip::socket::flag::kReuseport, true);

      switch (protocol) {
        case core::net::ip::protocol::kTcp:
          EXPECT_EQ(socket.connect(endpoint),
                    core::net::ip::socket::connection_status::kFailure);
          EXPECT_ANY_THROW(socket.connect(endpoint));
          break;
        case core::net::ip::protocol::kUdp:
          EXPECT_EQ(socket.connect(endpoint),
                    core::net::ip::socket::connection_status::kSuccess);
          EXPECT_EQ(socket.connect(endpoint),
                    core::net::ip::socket::connection_status::kSuccess);
          break;
      }
    }
  }
}

TEST(ip_socket, endpoints) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    const core::net::ip::endpoint bind_endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9997));
    const core::net::ip::endpoint connect_endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9996));
    for (const auto protocol :
         {core::net::ip::protocol::kTcp, core::net::ip::protocol::kUdp}) {
      core::net::ip::socket socket(protocol, version);
      socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::net::ip::socket::flag::kReuseport, true);

      EXPECT_NO_THROW(socket.get_bind_endpoint());
      socket.bind(bind_endpoint);
      EXPECT_EQ(socket.get_bind_endpoint(), bind_endpoint);
      EXPECT_ANY_THROW(socket.get_connect_endpoint());

      switch (protocol) {
        case core::net::ip::protocol::kTcp:
          EXPECT_EQ(socket.connect(connect_endpoint),
                    core::net::ip::socket::connection_status::kFailure);
          EXPECT_ANY_THROW(socket.get_connect_endpoint());
          break;
        case core::net::ip::protocol::kUdp:
          EXPECT_EQ(socket.connect(connect_endpoint),
                    core::net::ip::socket::connection_status::kSuccess);
          EXPECT_EQ(socket.get_connect_endpoint(), connect_endpoint);
          break;
      }
    }
  }
}

}  // namespace tests::net::ip

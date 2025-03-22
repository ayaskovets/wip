#include "ip/socket.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_socket, size) {
  static_assert(sizeof(core::ip::socket) == 4);
  static_assert(alignof(core::ip::socket) == 4);
}

TEST(ip_socket, flags) {
  for (const auto protocol :
       {core::ip::protocol::kTcp, core::ip::protocol::kUdp}) {
    for (const auto version :
         {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
      core::ip::socket socket(protocol, version);
      for (const auto flag : {core::ip::socket::flag::kNonblocking,
                              core::ip::socket::flag::kReuseaddr,
                              core::ip::socket::flag::kReuseport,
                              core::ip::socket::flag::kKeepalive}) {
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
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint endpoint(core::ip::address::kLocalhost(version),
                                      core::ip::port(9999));
    for (const auto protocol :
         {core::ip::protocol::kTcp, core::ip::protocol::kUdp}) {
      core::ip::socket socket(protocol, version);
      socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::ip::socket::flag::kReuseport, true);
      socket.bind(endpoint);
      EXPECT_ANY_THROW(socket.bind(endpoint));
    }
  }
}

TEST(ip_socket, connect) {
  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint endpoint(core::ip::address::kLocalhost(version),
                                      core::ip::port(9998));
    for (const auto protocol :
         {core::ip::protocol::kTcp, core::ip::protocol::kUdp}) {
      core::ip::socket socket(protocol, version);
      socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::ip::socket::flag::kReuseport, true);

      switch (protocol) {
        case core::ip::protocol::kTcp:
          EXPECT_EQ(socket.connect(endpoint),
                    core::ip::socket::connection_status::kFailure);
          EXPECT_ANY_THROW(socket.connect(endpoint));
          break;
        case core::ip::protocol::kUdp:
          EXPECT_EQ(socket.connect(endpoint),
                    core::ip::socket::connection_status::kSuccess);
          EXPECT_EQ(socket.connect(endpoint),
                    core::ip::socket::connection_status::kSuccess);
          break;
      }
    }
  }
}

TEST(ip_socket, endpoints) {
  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint bind_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9997));
    const core::ip::endpoint connect_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9996));
    for (const auto protocol :
         {core::ip::protocol::kTcp, core::ip::protocol::kUdp}) {
      core::ip::socket socket(protocol, version);
      socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
      socket.set_flag(core::ip::socket::flag::kReuseport, true);

      EXPECT_NO_THROW(socket.get_bind_endpoint());
      socket.bind(bind_endpoint);
      EXPECT_EQ(socket.get_bind_endpoint(), bind_endpoint);
      EXPECT_ANY_THROW(socket.get_connect_endpoint());

      switch (protocol) {
        case core::ip::protocol::kTcp:
          EXPECT_EQ(socket.connect(connect_endpoint),
                    core::ip::socket::connection_status::kFailure);
          EXPECT_ANY_THROW(socket.get_connect_endpoint());
          break;
        case core::ip::protocol::kUdp:
          EXPECT_EQ(socket.connect(connect_endpoint),
                    core::ip::socket::connection_status::kSuccess);
          EXPECT_EQ(socket.get_connect_endpoint(), connect_endpoint);
          break;
      }
    }
  }
}

}  // namespace tests::ip

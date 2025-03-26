#include "net/ip/tcp/socket.hpp"

#include <gtest/gtest.h>

#include <thread>

namespace tests::net::ip::tcp {

TEST(ip_tcp_socket, size) {
  static_assert(sizeof(core::net::ip::tcp::socket) == 4);
  static_assert(alignof(core::net::ip::tcp::socket) == 4);
}

TEST(ip_tcp_socket, blocking_send_receive_error) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    core::net::ip::tcp::socket socket(version);
    socket.set_flag(core::net::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseport, true);

    std::vector<std::uint8_t> bytes;
    EXPECT_ANY_THROW(socket.send(bytes));
    EXPECT_ANY_THROW(socket.receive(bytes));
  }
}

TEST(ip_tcp_socket, nonblocking_send_receive_error) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    core::net::ip::tcp::socket socket(version);
    socket.set_flag(core::net::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseport, true);

    std::vector<std::uint8_t> bytes;
    EXPECT_ANY_THROW(socket.send(bytes));
    EXPECT_ANY_THROW(socket.receive(bytes));
  }
}

TEST(ip_tcp_socket, accept_error) {
  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    core::net::ip::tcp::socket socket(version);
    socket.set_flag(core::net::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::net::ip::socket::flag::kReuseport, true);

    EXPECT_ANY_THROW(socket.accept());
    socket.listen(1);
    EXPECT_NO_THROW(socket.accept());
  }
}

TEST(ip_tcp_socket, blocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    const core::net::ip::endpoint endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9995));

    core::net::ip::tcp::socket server(version);
    server.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    server.set_flag(core::net::ip::socket::flag::kReuseport, true);
    server.bind(endpoint);
    server.listen(1);

    core::net::ip::tcp::socket client(version);
    client.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::net::ip::socket::flag::kReuseport, true);

    std::thread server_thread([&server] {
      const std::optional<core::net::ip::tcp::socket> peer = server.accept();
      EXPECT_TRUE(peer.has_value());

      std::vector<std::uint8_t> buffer(kBuffer.size());
      EXPECT_EQ(peer->receive(buffer), kBuffer.size());
      EXPECT_EQ(buffer, kBuffer);
      EXPECT_EQ(peer->send(kBuffer), kBuffer.size());
    });

    EXPECT_EQ(client.connect(endpoint),
              core::net::ip::socket::connection_status::kSuccess);

    std::vector<std::uint8_t> buffer(kBuffer.size());
    EXPECT_EQ(client.send(kBuffer), kBuffer.size());
    EXPECT_EQ(client.receive(buffer), kBuffer.size());
    EXPECT_EQ(buffer, kBuffer);
    server_thread.join();
  }
}

TEST(ip_tcp_socket, nonblocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::net::ip::version::kIPv4, core::net::ip::version::kIPv6}) {
    const core::net::ip::endpoint endpoint(
        core::net::ip::address::kLocalhost(version), core::net::ip::port(9996));

    core::net::ip::tcp::socket server(version);
    server.set_flag(core::net::ip::socket::flag::kNonblocking, true);
    server.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    server.set_flag(core::net::ip::socket::flag::kReuseport, true);

    core::net::ip::tcp::socket client(version);
    client.set_flag(core::net::ip::socket::flag::kNonblocking, true);
    client.set_flag(core::net::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::net::ip::socket::flag::kReuseport, true);

    server.bind(endpoint);
    server.listen(1);
    EXPECT_EQ(client.connect(endpoint),
              core::net::ip::socket::connection_status::kPending);

    std::optional<core::net::ip::tcp::socket> peer;
    while (!(peer = server.accept()).has_value()) {
    }

    std::vector<std::uint8_t> buffer(kBuffer.size());
    EXPECT_EQ(peer->send(kBuffer), kBuffer.size());
    while (client.receive(buffer) != kBuffer.size()) {
    }
    EXPECT_EQ(buffer, kBuffer);
  }
}

}  // namespace tests::net::ip::tcp

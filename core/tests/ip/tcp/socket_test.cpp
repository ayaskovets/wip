#include "ip/tcp/socket.hpp"

#include <gtest/gtest.h>

#include <latch>
#include <thread>

namespace tests::ip::tcp {

TEST(ip_tcp_socket, size) {
  static_assert(sizeof(core::ip::tcp::socket) == 4);
  static_assert(alignof(core::ip::tcp::socket) == 4);
}

TEST(ip_tcp_socket, blocking_send_receive_error) {
  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    core::ip::tcp::socket socket(version);
    socket.set_flag(core::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::ip::socket::flag::kReuseport, true);

    std::vector<std::uint8_t> bytes;
    EXPECT_ANY_THROW(socket.send(bytes));
    EXPECT_ANY_THROW(socket.receive(bytes));
  }
}

TEST(ip_tcp_socket, nonblocking_send_receive_error) {
  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    core::ip::tcp::socket socket(version);
    socket.set_flag(core::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::ip::socket::flag::kReuseport, true);

    std::vector<std::uint8_t> bytes;
    EXPECT_ANY_THROW(socket.send(bytes));
    EXPECT_ANY_THROW(socket.receive(bytes));
  }
}

TEST(ip_tcp_socket, blocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint endpoint(core::ip::address::kLocalhost(version),
                                      core::ip::port(9995));

    std::latch latch(2);
    std::thread server([version, &endpoint, &latch] {
      core::ip::tcp::socket server(version);
      server.set_flag(core::ip::socket::flag::kReuseaddr, true);
      server.set_flag(core::ip::socket::flag::kReuseport, true);
      EXPECT_ANY_THROW(server.accept());
      server.bind(endpoint);
      server.listen(1);
      latch.count_down();

      const std::optional<core::ip::tcp::socket> peer = server.accept();
      EXPECT_TRUE(peer.has_value());

      std::vector<std::uint8_t> buffer(kBuffer.size());
      EXPECT_EQ(peer->receive(buffer), kBuffer.size());
      EXPECT_EQ(buffer, kBuffer);
      EXPECT_EQ(peer->send(kBuffer), kBuffer.size());
    });

    core::ip::tcp::socket client(version);
    client.set_flag(core::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::ip::socket::flag::kReuseport, true);

    latch.arrive_and_wait();
    EXPECT_EQ(client.connect(endpoint),
              core::ip::socket::connection_status::kSuccess);

    std::vector<std::uint8_t> buffer(kBuffer.size());
    EXPECT_EQ(client.send(kBuffer), kBuffer.size());
    EXPECT_EQ(client.receive(buffer), kBuffer.size());
    EXPECT_EQ(buffer, kBuffer);
    server.join();
  }
}

TEST(ip_tcp_socket, nonblocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::ip::version::kIpV4, core::ip::version::kIpV6}) {
    const core::ip::endpoint endpoint(core::ip::address::kLocalhost(version),
                                      core::ip::port(9996));

    core::ip::tcp::socket server(version);
    server.set_flag(core::ip::socket::flag::kNonblocking, true);
    server.set_flag(core::ip::socket::flag::kReuseaddr, true);
    server.set_flag(core::ip::socket::flag::kReuseport, true);
    EXPECT_ANY_THROW(server.accept());
    server.bind(endpoint);
    server.listen(1);
    EXPECT_FALSE(server.accept().has_value());

    core::ip::tcp::socket client(version);
    client.set_flag(core::ip::socket::flag::kNonblocking, true);
    client.set_flag(core::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::ip::socket::flag::kReuseport, true);
    EXPECT_EQ(client.connect(endpoint),
              core::ip::socket::connection_status::kPending);

    std::optional<core::ip::tcp::socket> peer;
    while (!(peer = server.accept()).has_value()) {
    }

    std::vector<std::uint8_t> buffer(kBuffer.size());
    EXPECT_EQ(peer->send(kBuffer), kBuffer.size());
    while (client.receive(buffer) != kBuffer.size()) {
    }
    EXPECT_EQ(buffer, kBuffer);
  }
}

}  // namespace tests::ip::tcp

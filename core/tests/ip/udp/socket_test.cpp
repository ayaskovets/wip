#include "ip/udp/socket.hpp"

#include <gtest/gtest.h>

#include <thread>

namespace tests::ip::udp {

TEST(ip_udp_socket, size) {
  static_assert(sizeof(core::ip::udp::socket) == 4);
  static_assert(alignof(core::ip::udp::socket) == 4);
}

TEST(ip_udp_socket, nonblocking_send_receive_error) {
  for (const auto version :
       {core::ip::version::kIPv4, core::ip::version::kIPv6}) {
    core::ip::udp::socket socket(version);
    socket.set_flag(core::ip::socket::flag::kNonblocking, true);
    socket.set_flag(core::ip::socket::flag::kReuseaddr, true);
    socket.set_flag(core::ip::socket::flag::kReuseport, true);

    std::vector<std::uint8_t> bytes;
    EXPECT_ANY_THROW(socket.send(bytes));
    EXPECT_EQ(socket.receive(bytes), 0);
  }
}

TEST(ip_udp_socket, blocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::ip::version::kIPv4, core::ip::version::kIPv6}) {
    const core::ip::endpoint server_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9995));
    const core::ip::endpoint client_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9996));

    core::ip::udp::socket server(version);
    server.set_flag(core::ip::socket::flag::kReuseaddr, true);
    server.set_flag(core::ip::socket::flag::kReuseport, true);
    server.bind(server_endpoint);

    core::ip::udp::socket client(version);
    client.set_flag(core::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::ip::socket::flag::kReuseport, true);
    client.bind(client_endpoint);

    std::thread server_thread([version, &server, &client_endpoint] {
      core::ip::endpoint peer(core::ip::address::kNonRoutable(version),
                              core::ip::port(0));
      std::vector<std::uint8_t> buffer(kBuffer.size());
      EXPECT_EQ(server.receive_from(buffer, peer), kBuffer.size());
      EXPECT_EQ(buffer, kBuffer);
      EXPECT_EQ(peer, client_endpoint);
      EXPECT_EQ(server.send_to(kBuffer, peer), kBuffer.size());

      EXPECT_EQ(server.receive(buffer), kBuffer.size());
      EXPECT_EQ(server.connect(client_endpoint),
                core::ip::socket::connection_status::kSuccess);
      EXPECT_EQ(server.send(kBuffer), kBuffer.size());
    });

    core::ip::endpoint peer(core::ip::address::kNonRoutable(version),
                            core::ip::port(0));
    std::vector<std::uint8_t> buffer(kBuffer.size());
    EXPECT_EQ(client.send_to(kBuffer, server_endpoint), kBuffer.size());
    EXPECT_EQ(client.receive_from(buffer, peer), kBuffer.size());
    EXPECT_EQ(buffer, kBuffer);
    EXPECT_EQ(peer, server_endpoint);
    EXPECT_EQ(client.connect(server_endpoint),
              core::ip::socket::connection_status::kSuccess);
    EXPECT_EQ(client.send(kBuffer), kBuffer.size());
    EXPECT_EQ(client.receive(buffer), kBuffer.size());
    server_thread.join();
  }
}

TEST(ip_udp_socket, nonblocking_echo_handshake) {
  static const std::vector<std::uint8_t> kBuffer{1, 2, 3};

  for (const auto version :
       {core::ip::version::kIPv4, core::ip::version::kIPv6}) {
    const core::ip::endpoint server_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9995));
    const core::ip::endpoint client_endpoint(
        core::ip::address::kLocalhost(version), core::ip::port(9996));

    core::ip::udp::socket server(version);
    server.set_flag(core::ip::socket::flag::kNonblocking, true);
    server.set_flag(core::ip::socket::flag::kReuseaddr, true);
    server.set_flag(core::ip::socket::flag::kReuseport, true);
    server.bind(server_endpoint);

    core::ip::udp::socket client(version);
    client.set_flag(core::ip::socket::flag::kNonblocking, true);
    client.set_flag(core::ip::socket::flag::kReuseaddr, true);
    client.set_flag(core::ip::socket::flag::kReuseport, true);
    client.bind(client_endpoint);

    std::vector<std::uint8_t> buffer(kBuffer.size());
    core::ip::endpoint peer(core::ip::address::kNonRoutable(version),
                            core::ip::port(0));
    EXPECT_EQ(server.send_to(kBuffer, client_endpoint), kBuffer.size());
    while (client.receive_from(buffer, peer) != kBuffer.size()) {
    }
    EXPECT_EQ(buffer, kBuffer);
    EXPECT_EQ(peer, server_endpoint);

    server.connect(client_endpoint);
    client.connect(server_endpoint);

    EXPECT_EQ(client.send(kBuffer), kBuffer.size());
    while (server.receive(buffer) != kBuffer.size()) {
    }
    EXPECT_EQ(buffer, kBuffer);
  }
}
}  // namespace tests::ip::udp

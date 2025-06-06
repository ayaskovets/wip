#include "net/inet/tcp/socket.hpp"

#include <gtest/gtest.h>

#include <thread>

#include "net/inet/sockaddr.hpp"

namespace tests::net::inet::tcp {

TEST(net_inet_tcp_socket, size) {
  static_assert(sizeof(core::net::inet::tcp::socket) == 4);
  static_assert(alignof(core::net::inet::tcp::socket) == 4);
}

TEST(net_inet_tcp_socket, blocking_send_receive_error) {
  core::net::inet::tcp::socket socket;
  socket.set_nonblock(true);
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);

  std::vector<std::byte> bytes;
  EXPECT_ANY_THROW(socket.send(bytes));
  EXPECT_ANY_THROW(socket.receive(bytes));
}

TEST(net_inet_tcp_socket, nonblocking_send_receive_error) {
  core::net::inet::tcp::socket socket;
  socket.set_nonblock(true);
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);

  std::vector<std::byte> bytes;
  EXPECT_ANY_THROW(socket.send(bytes));
  EXPECT_ANY_THROW(socket.receive(bytes));
}

TEST(net_inet_tcp_socket, bind) {
  const core::net::inet::sockaddr sockaddr(core::net::inet::ip::kLoopback(),
                                           core::net::inet::port(9999));

  core::net::inet::tcp::socket socket;
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);
  socket.bind(sockaddr);
  EXPECT_ANY_THROW(socket.bind(sockaddr));
}

TEST(net_inet_tcp_socket, connect) {
  const core::net::inet::sockaddr sockaddr(core::net::inet::ip::kLoopback(),
                                           core::net::inet::port(9998));

  core::net::inet::tcp::socket socket;
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);

  EXPECT_EQ(socket.connect(sockaddr),
            core::net::inet::tcp::socket::connection_status::kRefused);
  EXPECT_ANY_THROW(socket.connect(sockaddr));
}

TEST(net_inet_tcp_socket, sockaddrs) {
  const core::net::inet::sockaddr bind_sockaddr(
      core::net::inet::ip::kLoopback(), core::net::inet::port(9997));
  const core::net::inet::sockaddr connect_sockaddr(
      core::net::inet::ip::kLoopback(), core::net::inet::port(9996));

  core::net::inet::tcp::socket socket;
  socket.set_nonblock(true);
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);

  core::net::inet::sockaddr out_sockaddr(core::net::inet::ip::kAny(),
                                         core::net::inet::port(0));
  EXPECT_NO_THROW((socket.get_bind_sockaddr(out_sockaddr)));
  EXPECT_NE((socket.get_bind_sockaddr(out_sockaddr), out_sockaddr),
            bind_sockaddr);
  socket.bind(bind_sockaddr);
  EXPECT_EQ((socket.get_bind_sockaddr(out_sockaddr), out_sockaddr),
            bind_sockaddr);
  EXPECT_ANY_THROW(socket.get_connect_sockaddr(out_sockaddr));

  EXPECT_EQ(socket.connect(connect_sockaddr),
            core::net::inet::tcp::socket::connection_status::kPending);
  EXPECT_ANY_THROW(socket.get_connect_sockaddr(out_sockaddr));
}

TEST(net_inet_tcp_socket, accept_error) {
  core::net::inet::tcp::socket socket;
  socket.set_nonblock(true);
  socket.set_reuseaddr(true);
  socket.set_reuseport(true);

  core::net::inet::tcp::socket peer(core::utils::uninitialized_t{});
  EXPECT_ANY_THROW(socket.accept(peer));
  socket.listen(1);
  EXPECT_EQ(socket.accept(peer),
            core::net::inet::tcp::socket::accept_status::kEmptyQueue);
}

TEST(net_inet_tcp_socket, blocking_echo_handshake) {
  const std::vector<std::byte> kBuffer{std::byte(1), std::byte(2),
                                       std::byte(3)};

  const core::net::inet::sockaddr sockaddr(core::net::inet::ip::kLoopback(),
                                           core::net::inet::port(9995));

  core::net::inet::tcp::socket server;
  server.set_reuseaddr(true);
  server.set_reuseport(true);
  server.bind(sockaddr);
  server.listen(1);

  core::net::inet::tcp::socket client;
  client.set_reuseaddr(true);
  client.set_reuseport(true);

  std::thread server_thread([&server, &kBuffer] {
    core::net::inet::tcp::socket peer;
    EXPECT_EQ(server.accept(peer),
              core::net::inet::tcp::socket::accept_status::kSuccess);

    std::vector<std::byte> buffer(kBuffer.size());
    EXPECT_EQ(peer.receive(buffer), kBuffer.size());
    EXPECT_EQ(buffer, kBuffer);
    EXPECT_EQ(peer.send(kBuffer), kBuffer.size());
  });

  EXPECT_EQ(client.connect(sockaddr),
            core::net::inet::tcp::socket::connection_status::kSuccess);

  std::vector<std::byte> buffer(kBuffer.size());
  EXPECT_EQ(client.send(kBuffer), kBuffer.size());
  EXPECT_EQ(client.receive(buffer), kBuffer.size());
  EXPECT_EQ(buffer, kBuffer);
  server_thread.join();
}

TEST(net_inet_tcp_socket, nonblocking_echo_handshake) {
  const std::vector<std::byte> kBuffer{std::byte(1), std::byte(2),
                                       std::byte(3)};

  const core::net::inet::sockaddr sockaddr(core::net::inet::ip::kLoopback(),
                                           core::net::inet::port(9996));

  core::net::inet::tcp::socket server;
  server.set_nonblock(true);
  server.set_reuseaddr(true);
  server.set_reuseport(true);

  core::net::inet::tcp::socket client;
  client.set_nonblock(true);
  client.set_reuseaddr(true);
  client.set_reuseport(true);

  server.bind(sockaddr);
  server.listen(1);
  EXPECT_EQ(client.connect(sockaddr),
            core::net::inet::tcp::socket::connection_status::kPending);

  core::net::inet::tcp::socket peer(core::utils::uninitialized_t{});
  while (server.accept(peer) !=
         core::net::inet::tcp::socket::accept_status::kSuccess);

  std::vector<std::byte> buffer(kBuffer.size());
  EXPECT_EQ(peer.send(kBuffer), kBuffer.size());
  while (client.receive(buffer) != kBuffer.size());
  EXPECT_EQ(buffer, kBuffer);
}

}  // namespace tests::net::inet::tcp

#include "ip/tcp/connection.hpp"

#include <gtest/gtest.h>

#include <thread>

namespace tests::ip::tcp {

TEST(ip_tcp_connection, size) {
  static_assert(sizeof(core::ip::tcp::connection) == 4);
  static_assert(alignof(core::ip::tcp::connection) == 4);
}

TEST(ip_tcp_connection, blocking_successful_connection) {
  const core::ip::endpoint endpoint(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));

  std::thread client([&endpoint] {
    const core::ip::tcp::acceptor acceptor(endpoint, 1);
    core::ip::tcp::connection::accept(acceptor);
    core::ip::tcp::connection::accept(acceptor);
  });

  EXPECT_TRUE(core::ip::tcp::connection::connect(endpoint).has_value());
  EXPECT_TRUE(core::ip::tcp::connection::connect(endpoint).has_value());
  EXPECT_FALSE(core::ip::tcp::connection::connect(endpoint).has_value());
  client.join();
}

TEST(ip_tcp_connection, nonblocking_successful_connection) {
  const core::ip::endpoint endpoint(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));
  const core::ip::tcp::acceptor acceptor(endpoint, 1);

  const auto connection = core::ip::tcp::connection::try_connect(endpoint);
  EXPECT_TRUE(connection.has_value());
  // NOTE: time for accept not to return EAGAIN
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  EXPECT_TRUE(core::ip::tcp::connection::try_accept(acceptor).has_value());
}

// TODO: send/recv

}  // namespace tests::ip::tcp

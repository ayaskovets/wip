#include "net/ip/tcp/connection.hpp"

#include <gtest/gtest.h>

#include <thread>

namespace tests::net::ip::tcp {

TEST(net_ip_tcp_connection, size) {
  static_assert(sizeof(core::net::ip::tcp::connection) == 4);
  static_assert(alignof(core::net::ip::tcp::connection) == 4);
}

// TEST(net_ip_tcp_connection, blocking_successful_connection) {
//   const core::net::ip::endpoint endpoint(
//       core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
//       core::net::ip::port(9996));

//   std::thread client([&endpoint] {
//     const core::net::ip::tcp::acceptor acceptor(endpoint, 1);
//     core::net::ip::tcp::connection::accept(acceptor);
//     core::net::ip::tcp::connection::accept(acceptor);
//   });

//   EXPECT_TRUE(core::net::ip::tcp::connection::connect(endpoint).has_value());
//   EXPECT_TRUE(core::net::ip::tcp::connection::connect(endpoint).has_value());
//   EXPECT_FALSE(core::net::ip::tcp::connection::connect(endpoint).has_value());
//   client.join();
// }

// TEST(net_ip_tcp_connection, nonblocking_successful_connection) {
//   const core::net::ip::endpoint endpoint(
//       core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
//       core::net::ip::port(9996));
//   const core::net::ip::tcp::acceptor acceptor(endpoint, 1);

//   const auto connection =
//   core::net::ip::tcp::connection::try_connect(endpoint);
//   EXPECT_TRUE(connection.has_value());
//   // NOTE: time for accept not to return EAGAIN
//   std::this_thread::sleep_for(std::chrono::milliseconds(100));
//   EXPECT_TRUE(core::net::ip::tcp::connection::try_accept(acceptor).has_value());
// }

// TODO: send/receive on an established connection

}  // namespace tests::net::ip::tcp

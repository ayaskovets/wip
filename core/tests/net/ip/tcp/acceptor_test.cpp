#include "net/ip/tcp/acceptor.hpp"

#include <gtest/gtest.h>

namespace tests::net::ip::tcp {

TEST(ip_tcp_acceptor, size) {
  static_assert(sizeof(core::net::ip::tcp::acceptor) == 28);
  static_assert(alignof(core::net::ip::tcp::acceptor) == 4);
}

TEST(ip_tcp_acceptor, try_accept_ip4) {
  const core::net::ip::tcp::acceptor acceptor(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9995)),
      1);
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip_tcp_acceptor, try_accept_ip6) {
  const core::net::ip::tcp::acceptor acceptor(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv6),
          core::net::ip::port(9994)),
      1);
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip_tcp_acceptor, copy) {
  core::net::ip::tcp::acceptor original(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9993)),
      1);

  core::net::ip::tcp::acceptor copy(original);
  EXPECT_FALSE(original.try_accept().has_value());
  EXPECT_FALSE(copy.try_accept().has_value());
}

TEST(ip_tcp_acceptor, move_operations) {
  core::net::ip::tcp::acceptor moved_from1(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9992)),
      1);

  core::net::ip::tcp::acceptor moved_from2(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9991)),
      1);

  core::net::ip::tcp::acceptor acceptor(std::move(moved_from1));
  EXPECT_ANY_THROW(moved_from1.try_accept());
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

}  // namespace tests::net::ip::tcp

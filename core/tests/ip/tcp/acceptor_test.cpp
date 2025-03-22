#include "ip/tcp/acceptor.hpp"

#include <gtest/gtest.h>

namespace tests::ip::tcp {

TEST(ip_tcp_acceptor, size) {
  static_assert(sizeof(core::ip::tcp::acceptor) == 28);
  static_assert(alignof(core::ip::tcp::acceptor) == 4);
}

TEST(ip_tcp_acceptor, try_accept_ip4) {
  const core::ip::tcp::acceptor acceptor(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV4),
          core::ip::port(9995)),
      1);
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip_tcp_acceptor, try_accept_ip6) {
  const core::ip::tcp::acceptor acceptor(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV6),
          core::ip::port(9994)),
      1);
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip_tcp_acceptor, copy) {
  core::ip::tcp::acceptor original(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV4),
          core::ip::port(9993)),
      1);

  core::ip::tcp::acceptor copy(original);
  EXPECT_FALSE(original.try_accept().has_value());
  EXPECT_FALSE(copy.try_accept().has_value());
}

TEST(ip_tcp_acceptor, move_operations) {
  core::ip::tcp::acceptor moved_from1(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV4),
          core::ip::port(9992)),
      1);

  core::ip::tcp::acceptor moved_from2(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV4),
          core::ip::port(9991)),
      1);

  core::ip::tcp::acceptor acceptor(std::move(moved_from1));
  EXPECT_ANY_THROW(moved_from1.try_accept());
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

}  // namespace tests::ip::tcp

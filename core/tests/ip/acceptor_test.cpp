#include "ip/acceptor.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip, acceptor_try_accept_ip4) {
  const auto acceptor = core::ip::acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip, acceptor_try_accept_ip6) {
  const auto acceptor = core::ip::acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV6),
      core::ip::port(9996));
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

TEST(ip, acceptor_copy) {
  auto original = core::ip::acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));

  core::ip::acceptor copy = original;
  EXPECT_FALSE(original.try_accept().has_value());
  EXPECT_FALSE(copy.try_accept().has_value());
}

TEST(ip, acceptor_move_operations) {
  auto moved_from1 = core::ip::acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));

  auto moved_from2 = core::ip::acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9997));

  core::ip::acceptor acceptor = std::move(moved_from1);
  EXPECT_ANY_THROW(moved_from1.try_accept());
  EXPECT_FALSE(acceptor.try_accept().has_value());

  acceptor = std::move(moved_from2);
  EXPECT_ANY_THROW(moved_from2.try_accept());
  EXPECT_FALSE(acceptor.try_accept().has_value());
}

}  // namespace tests::ip

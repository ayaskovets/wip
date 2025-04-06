#include "net/unix/sockaddr.hpp"

#include <gtest/gtest.h>

namespace tests::net::unix {

TEST(net_unix_sockaddr, size) {
  static_assert(sizeof(core::net::unix::sockaddr) == 16);
  static_assert(alignof(core::net::unix::sockaddr) == 8);
}

TEST(net_unix_sockaddr, construction) {
  EXPECT_NO_THROW(core::net::unix::sockaddr(""));
  EXPECT_NO_THROW(core::net::unix::sockaddr("/path/to/a/file"));
  EXPECT_ANY_THROW(core::net::unix::sockaddr(
      "/loooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
      "oooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo"
      "oooooooooooooooooooooooooong"));
}

TEST(net_unix_sockaddr, equality) {
  EXPECT_EQ(core::net::unix::sockaddr::kEmpty(),
            core::net::unix::sockaddr::kEmpty());
  EXPECT_EQ(core::net::unix::sockaddr("/path/to/a/file"),
            core::net::unix::sockaddr("/path/to/a/file"));
  EXPECT_NE(core::net::unix::sockaddr("/path/to/a/file"),
            core::net::unix::sockaddr("/path/to/a/file2"));
  EXPECT_NE(core::net::unix::sockaddr("/path/to/a/file2"),
            core::net::unix::sockaddr("/path/to/a/file"));
}

TEST(net_unix_sockaddr, to_string) {
  EXPECT_EQ(
      core::net::unix::sockaddr("net_unix_sockaddr_to_string").to_string(),
      "net_unix_sockaddr_to_string");
}

}  // namespace tests::net::unix

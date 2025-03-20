#include "io/fd.hpp"

#include <gtest/gtest.h>

namespace tests::io {

TEST(io, fd_size) {
  static_assert(sizeof(core::io::fd) == 4);
  static_assert(alignof(core::io::fd) == 4);
}

TEST(io, fd_manual_close) {
  core::io::fd fd = core::io::fd::kStdin();
  EXPECT_NO_THROW(fd.close());
  EXPECT_NO_THROW(fd.close());
  EXPECT_NO_THROW(fd.close());
}

TEST(io, fd_transfer_ownership) {
  core::io::fd moved_from = core::io::fd::kStdin();
  core::io::fd moved_to = std::move(moved_from);
  EXPECT_NE(moved_from, moved_to);
  EXPECT_NO_THROW(moved_from.close());
  EXPECT_NO_THROW(moved_from.close());
  EXPECT_NE(moved_from, moved_to);
  EXPECT_NO_THROW(moved_to.close());
  EXPECT_NE(moved_from, moved_to);
}

TEST(io, fd_duplicate) {
  core::io::fd original = core::io::fd::kStdin();
  core::io::fd copy = original;
  EXPECT_NO_THROW(original.close());
  EXPECT_NO_THROW(copy.close());
  EXPECT_NE(original, copy);
}

TEST(io, fd_equality) {
  EXPECT_EQ(core::io::fd::kStdin(), core::io::fd::kStdin());
  EXPECT_EQ(core::io::fd::kStdout(), core::io::fd::kStdout());
  EXPECT_EQ(core::io::fd::kStderr(), core::io::fd::kStderr());
  EXPECT_NE(core::io::fd::kStdin(), core::io::fd::kStdout());
  EXPECT_NE(core::io::fd::kStdout(), core::io::fd::kStderr());
}

}  // namespace tests::io

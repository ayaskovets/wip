#include "utils/aligned.hpp"

#include <gtest/gtest.h>

namespace tests::utils {

TEST(utils_aligned, size) {
  static_assert(sizeof(core::utils::aligned<std::size_t, 32>) == 32);
  static_assert(alignof(core::utils::aligned<std::size_t, 32>) == 32);
}

TEST(utils_aligned, constructor) {
  core::utils::aligned<std::string, 32> aligned{"hello"};
  EXPECT_EQ(aligned.value, "hello");
}

}  // namespace tests::utils

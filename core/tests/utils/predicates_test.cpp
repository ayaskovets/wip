#include "utils/predicates.hpp"

#include <gtest/gtest.h>

namespace tests::utils {

TEST(utils, is_power_of_two) {
  static_assert(!core::utils::is_power_of_two(0));

  constexpr std::array<int, 14> kPowersOfTwo = {
      1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
  for (int i = 0; i < 10000; ++i) {
    if (std::find(kPowersOfTwo.begin(), kPowersOfTwo.end(), i) ==
        kPowersOfTwo.end()) {
      EXPECT_FALSE(core::utils::is_power_of_two(i));
    } else {
      EXPECT_TRUE(core::utils::is_power_of_two(i));
    }
  }
}

}  // namespace tests::utils

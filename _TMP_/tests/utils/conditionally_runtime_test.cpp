#include "utils/conditionally_runtime.hpp"

#include <gtest/gtest.h>

namespace tests::utils {

TEST(_TMP__utils, size) {
  static_assert(
      sizeof(_TMP_::utils::conditionally_runtime<std::size_t, false, 42>) == 1);
  static_assert(sizeof(_TMP_::utils::conditionally_runtime<int, true, 42>) ==
                sizeof(int));
}

TEST(_TMP__utils, constructor) {
  _TMP_::utils::conditionally_runtime<std::size_t, false, 42>();
  _TMP_::utils::conditionally_runtime<std::size_t, true>(42);
}

TEST(_TMP__utils, time_star_operator) {
  {
    _TMP_::utils::conditionally_runtime<int, false, 1> v;
    static_assert(std::is_same_v<decltype(*v), const int&>);
    EXPECT_EQ(*v, 1);
  }
  {
    _TMP_::utils::conditionally_runtime<int, true> v(3);
    static_assert(std::is_same_v<decltype(*v), int&>);
    EXPECT_EQ(*v, 3);
  }
  {
    const _TMP_::utils::conditionally_runtime<int, true> v(3);
    static_assert(std::is_same_v<decltype(*v), const int&>);
    EXPECT_EQ(*v, 3);
  }
}

TEST(_TMP__utils, assignment) {
  {
    _TMP_::utils::conditionally_runtime<int, true> v(4);
    EXPECT_EQ(*v, 4);
    *v = 5;
    EXPECT_EQ(*v, 5);
  }
  {
    _TMP_::utils::conditionally_runtime<int, true> v1(4);
    _TMP_::utils::conditionally_runtime<int, true> v2(5);
    EXPECT_EQ(*v1, 4);
    v1 = v2;
    EXPECT_EQ(*v1, 5);
  }
}

}  // namespace tests::utils
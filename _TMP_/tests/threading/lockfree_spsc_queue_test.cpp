#include "threading/lockfree_spsc_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(_TMP__threading, lockfree_spsc_queue_size) {
  static_assert(sizeof(_TMP_::threading::lockfree_spsc_queue<int, 128>) == 320);
  static_assert(alignof(_TMP_::threading::lockfree_spsc_queue<int, 128>) == 64);

  static_assert(sizeof(_TMP_::threading::lockfree_spsc_queue<int>) == 320);
  static_assert(alignof(_TMP_::threading::lockfree_spsc_queue<int>) == 64);
}

TEST(_TMP__threading, lockfree_spsc_queue_size_capacity) {
  EXPECT_EQ(_TMP_::threading::lockfree_spsc_queue<std::string>(128).capacity(),
            128);
  EXPECT_EQ(_TMP_::threading::lockfree_spsc_queue<std::string>(64).capacity(),
            64);
}

TEST(_TMP__threading, lockfree_spsc_queue_sanity_check) {
  _TMP_::threading::lockfree_spsc_queue<int> queue(2);

  EXPECT_FALSE(queue.try_pop().has_value());
  EXPECT_TRUE(queue.try_push(1));
  EXPECT_TRUE(queue.try_push(2));
  EXPECT_FALSE(queue.try_push(3));
  EXPECT_EQ(queue.try_pop(), 1);
  EXPECT_TRUE(queue.try_push(4));
  EXPECT_FALSE(queue.try_push(5));
  EXPECT_EQ(queue.try_pop(), 2);
  EXPECT_EQ(queue.try_pop(), 4);
  EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(_TMP__threading, lockfree_spsc_queue_rollover) {
  _TMP_::threading::lockfree_spsc_queue<int> queue(5);

  for (std::size_t i = 0; i < 100; ++i) {
    EXPECT_TRUE(queue.try_push(i));
    EXPECT_EQ(queue.try_pop(), i);
  }
}

TEST(_TMP__threading, lockfree_spsc_queue_non_copyable_item_type) {
  struct non_copyable {
    constexpr non_copyable() noexcept = default;
    constexpr non_copyable(const non_copyable&) = delete;
    constexpr non_copyable& operator=(const non_copyable&) = delete;
    constexpr non_copyable(non_copyable&&) = default;
    constexpr non_copyable& operator=(non_copyable&&) = default;
  };

  _TMP_::threading::lockfree_spsc_queue<non_copyable, 1> queue;
  {
    queue.try_push(non_copyable{});
    [[maybe_unused]] const auto value = queue.try_pop();
  }
}

class _TMP__threading_lockfree_spsc_queue
    : public ::testing::TestWithParam<std::tuple<std::size_t, std::size_t>> {};

TEST_P(_TMP__threading_lockfree_spsc_queue, lockfree_spsc_queue_workload) {
  const auto& [items_size, queue_size] = GetParam();

  std::vector<int> items_to_push(items_size);
  std::iota(items_to_push.begin(), items_to_push.end(), 0);

  _TMP_::threading::lockfree_spsc_queue<int> queue(queue_size);

  std::vector<int> popped_items;
  popped_items.reserve(items_size);

  std::latch latch(2);

  std::thread producer([&latch, &queue, items_size, &items_to_push] {
    latch.arrive_and_wait();
    std::size_t i = 0;
    while (i < items_size) {
      if (queue.try_push(items_to_push[i])) {
        ++i;
      }
    }
  });

  std::thread consumer([&latch, &queue, items_size, &popped_items] {
    latch.arrive_and_wait();
    while (popped_items.size() != items_size) {
      if (const auto item = queue.try_pop(); item.has_value()) {
        popped_items.push_back(*item);
      }
    }
  });

  producer.join();
  consumer.join();

  EXPECT_EQ(popped_items.size(), items_to_push.size());
  EXPECT_EQ(popped_items, items_to_push);
}

INSTANTIATE_TEST_SUITE_P(
    _TMP__threading_lockfree_spsc_queue, _TMP__threading_lockfree_spsc_queue,
    ::testing::Values(std::make_tuple(5, 3), std::make_tuple(8, 4),
                      std::make_tuple(5, 1), std::make_tuple(6, 2),
                      std::make_tuple(7, 2), std::make_tuple(10, 1),
                      std::make_tuple(20, 2), std::make_tuple(100, 10),
                      std::make_tuple(10000, 100)));

}  // namespace tests::threading

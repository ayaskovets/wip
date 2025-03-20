#include "threading/locked_mpmc_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(threading, locked_mpmc_queue_size) {
  static_assert(sizeof(core::threading::locked_mpmc_queue<int, 10>) == 208);
  static_assert(alignof(core::threading::locked_mpmc_queue<int, 10>) == 8);

  static_assert(sizeof(core::threading::locked_mpmc_queue<int>) == 216);
  static_assert(alignof(core::threading::locked_mpmc_queue<int>) == 8);
}

TEST(threading, locked_mpmc_queue_size_capacity) {
  {
    core::threading::locked_mpmc_queue<std::string> queue(101);
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 0);

    queue.push("42");
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 1);

    EXPECT_EQ(queue.pop(), "42");
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 0);
  }
  {
    core::threading::locked_mpmc_queue<std::string, 101> queue;
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 0);

    queue.push("42");
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 1);

    EXPECT_EQ(queue.pop(), "42");
    EXPECT_EQ(queue.capacity(), 101);
    EXPECT_EQ(queue.size(), 0);
  }
}

TEST(threading, locked_mpmc_queue_blocking_push) {
  core::threading::locked_mpmc_queue<int> queue(2);

  queue.push(1);
  queue.push(2);

  std::thread consumer([&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(queue.pop(), 1);
  });

  queue.push(3);
  EXPECT_EQ(queue.pop(), 2);

  consumer.join();
}

TEST(threading, locked_mpmc_queue_blocking_pop) {
  core::threading::locked_mpmc_queue<int> queue(1);

  std::thread producer([&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.push(42);
  });

  EXPECT_EQ(queue.pop(), 42);

  producer.join();
}

TEST(threading, locked_mpmc_queue_unblocking_pop) {
  core::threading::locked_mpmc_queue<int> queue(2);

  queue.push(1);
  queue.push(2);

  std::thread consumer([&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(queue.pop(), 1);
  });

  queue.push(3);
  EXPECT_EQ(queue.pop(), 2);

  consumer.join();
}

TEST(threading, locked_mpmc_queue_non_copyable_item_type) {
  struct non_copyable {
    constexpr non_copyable() noexcept = default;
    constexpr non_copyable(const non_copyable&) = delete;
    constexpr non_copyable& operator=(const non_copyable&) = delete;
    constexpr non_copyable(non_copyable&&) = default;
    constexpr non_copyable& operator=(non_copyable&&) = default;
  };

  core::threading::locked_mpmc_queue<non_copyable> queue(1);
  {
    queue.push(non_copyable{});
    [[maybe_unused]] const auto value = queue.pop();
  }
}

class threading_locked_mpmc_queue_
    : public ::testing::TestWithParam<
          std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> {};

TEST_P(threading_locked_mpmc_queue_, locked_mpmc_queue_workload) {
  const auto& [items_size, queue_size, producers, consumers] = GetParam();

  std::vector<int> items_to_push(items_size);
  std::iota(items_to_push.begin(), items_to_push.end(), 0);

  core::threading::locked_mpmc_queue<int> queue(queue_size);

  std::atomic<std::size_t> pushed = 0;

  std::mutex mutex;
  std::vector<int> popped_items;
  popped_items.reserve(items_to_push.size());

  std::latch latch(producers + consumers);
  std::vector<std::thread> threads;
  threads.reserve(producers + consumers);

  const auto producer = [&latch, &queue, &items_to_push, &pushed] {
    latch.arrive_and_wait();
    while (true) {
      const std::size_t index = pushed.fetch_add(1, std::memory_order::relaxed);
      if (index >= items_to_push.size()) {
        return;
      }
      queue.push(items_to_push[index]);
    }
  };

  const auto consumer = [&latch, &queue, &items_to_push, &mutex,
                         &popped_items] {
    latch.arrive_and_wait();
    while (true) {
      std::unique_lock<std::mutex> lock(mutex);
      if (popped_items.size() == items_to_push.size()) {
        return;
      }
      popped_items.push_back(queue.pop());
    }
  };

  for (std::size_t i = 0; i < producers; ++i) {
    threads.emplace_back(producer);
  }
  for (std::size_t i = 0; i < consumers; ++i) {
    threads.emplace_back(consumer);
  }
  for (auto& thread : threads) {
    thread.join();
  }

  std::sort(std::begin(popped_items), std::end(popped_items));
  EXPECT_EQ(popped_items.size(), items_to_push.size());
  EXPECT_EQ(popped_items, items_to_push);
}

INSTANTIATE_TEST_SUITE_P(threading_locked_mpmc_queue_,
                         threading_locked_mpmc_queue_,
                         ::testing::Values(std::make_tuple(5, 3, 1, 1),
                                           std::make_tuple(100, 10, 4, 1),
                                           std::make_tuple(100, 10, 1, 4),
                                           std::make_tuple(10000, 100, 4, 4)));

}  // namespace tests::threading

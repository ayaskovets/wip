#include "threading/locked_mpmc_queue.hpp"

#include <gtest/gtest.h>

#include <format>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(threading_locked_mpmc_queue, size) {
  static_assert(sizeof(core::threading::locked_mpmc_queue<int, 10>) == 216);
  static_assert(alignof(core::threading::locked_mpmc_queue<int, 10>) == 8);

  static_assert(sizeof(core::threading::locked_mpmc_queue<int>) == 224);
  static_assert(alignof(core::threading::locked_mpmc_queue<int>) == 8);
}

TEST(threading_locked_mpmc_queue, minimal_capacity) {
  EXPECT_ANY_THROW(core::threading::locked_mpmc_queue<int> queue(0));
}

TEST(threading_locked_mpmc_queue, capacity) {
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

TEST(threading_locked_mpmc_queue, blocking_push) {
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

TEST(threading_locked_mpmc_queue, blocking_pop) {
  core::threading::locked_mpmc_queue<int> queue(1);

  std::thread producer([&queue] {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.push(42);
  });

  EXPECT_EQ(queue.pop(), 42);

  producer.join();
}

TEST(threading_locked_mpmc_queue, unblocking_pop) {
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

TEST(threading_locked_mpmc_queue, producer_request_stop) {
  std::thread consumer;
  core::threading::locked_mpmc_queue<int, 1> queue;
  consumer = std::thread([&queue] {
    EXPECT_NO_THROW(queue.push(1));
    EXPECT_ANY_THROW(queue.push(2));
  });
  while (queue.size() == 0) {
  }
  queue.request_stop();
  consumer.join();
}

TEST(threading_locked_mpmc_queue, consumer_request_stop) {
  std::thread consumer;
  core::threading::locked_mpmc_queue<int, 1> queue;
  consumer = std::thread([&queue] { EXPECT_ANY_THROW(queue.pop()); });
  queue.request_stop();
  consumer.join();
}

TEST(threading_locked_mpmc_queue, non_copyable_item_type) {
  core::threading::locked_mpmc_queue<std::unique_ptr<int>> queue(1);
  queue.push(std::unique_ptr<int>{});
  [[maybe_unused]] const auto value = queue.pop();
}

TEST(threading_locked_mpmc_queue, allocator) {
  std::unordered_map<std::uint32_t*, std::size_t> allocations;

  {
    class allocator : public std::allocator<std::uint32_t> {
     public:
      explicit allocator(
          std::unordered_map<std::uint32_t*, std::size_t>& allocations)
          : allocations_(allocations) {}

      std::uint32_t* allocate(std::size_t n) {
        std::uint32_t* ptr = std::allocator<std::uint32_t>::allocate(n);
        allocations_[ptr] = n;
        return ptr;
      }
      void deallocate(std::uint32_t* ptr, std::size_t n) {
        if ((allocations_[ptr] -= n) == 0) {
          allocations_.erase(ptr);
        }
        std::allocator<std::uint32_t>::deallocate(ptr, n);
      }

     private:
      std::unordered_map<std::uint32_t*, std::size_t>& allocations_;
    };

    allocator alloc(allocations);
    core::threading::locked_mpmc_queue<std::uint32_t, 2, allocator> queue(
        alloc);

    queue.push(1);
    queue.push(2);
    EXPECT_EQ(queue.pop(), 1);
    queue.push(4);
    EXPECT_EQ(queue.pop(), 2);
    EXPECT_EQ(queue.pop(), 4);
  }
  EXPECT_TRUE(allocations.empty());
}

class threading_locked_mpmc_queue
    : public ::testing::TestWithParam<
          std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> {};

TEST_P(threading_locked_mpmc_queue, workload) {
  const auto& [items_size, queue_size, producers, consumers] = GetParam();

  core::threading::locked_mpmc_queue<int> queue(queue_size);

  std::vector<int> items_to_push(items_size);
  std::iota(items_to_push.begin(), items_to_push.end(), 0);

  std::vector<int> popped_items(items_size);

  std::atomic<std::size_t> pushed_items_count = 0;
  std::atomic<std::size_t> popped_items_count = 0;

  std::latch latch(producers + consumers);
  std::vector<std::thread> threads;
  threads.reserve(producers + consumers);

  const auto producer = [&latch, &queue, &items_to_push, &pushed_items_count] {
    latch.arrive_and_wait();
    while (true) {
      const std::size_t index =
          pushed_items_count.fetch_add(1, std::memory_order::relaxed);
      if (index >= items_to_push.size()) {
        return;
      }
      queue.push(items_to_push[index]);
    }
  };

  const auto consumer = [&latch, &queue, &items_to_push, &popped_items_count,
                         &popped_items]() {
    latch.arrive_and_wait();
    while (true) {
      const std::size_t index =
          popped_items_count.fetch_add(1, std::memory_order::relaxed);
      if (index >= items_to_push.size()) {
        return;
      }
      popped_items[index] = queue.pop();
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

  std::sort(popped_items.begin(), popped_items.end());
  EXPECT_EQ(popped_items.size(), popped_items.size());
  EXPECT_EQ(std::vector<int>(popped_items.begin(), popped_items.end()),
            items_to_push);
}

INSTANTIATE_TEST_SUITE_P(
    threading_locked_mpmc_queue, threading_locked_mpmc_queue,
    ::testing::Values(std::make_tuple(5, 3, 1, 1),
                      std::make_tuple(20, 10, 2, 1),
                      std::make_tuple(100, 10, 4, 1),
                      std::make_tuple(100, 10, 1, 4),
                      std::make_tuple(10000, 100, 4, 4)),
    ([](const auto& info) {
      const auto& [items_size, queue_size, producers, consumers] = info.param;
      return std::format(
          "{}_items_size__{}_queue_size__{}_producers__{}_consumers",
          items_size, queue_size, producers, consumers);
    }));

}  // namespace tests::threading

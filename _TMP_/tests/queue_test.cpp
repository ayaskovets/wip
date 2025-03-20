#include <gtest/gtest.h>

#include <atomic>
#include <latch>
#include <numeric>
#include <thread>

#include "queue/mpmc_queue.hpp"

namespace tests::logging {

TEST(_TMP__queue, single_thread_try_pop) {
  _TMP_::queue::mpmc_queue<int> queue(1);
  EXPECT_FALSE(queue.try_pop().has_value());
  queue.push(42);
  EXPECT_EQ(queue.try_pop(), 42);
  EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(_TMP__queue, blocking_push) {
  _TMP_::queue::mpmc_queue<int> queue(2);

  queue.push(1);
  queue.push(2);

  std::thread consumer([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(queue.pop(), 1);
  });

  queue.push(3);
  EXPECT_EQ(queue.pop(), 2);

  consumer.join();
}

TEST(_TMP__queue, blocking_pop) {
  _TMP_::queue::mpmc_queue<int> queue(1);

  std::thread producer([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.push(42);
  });

  EXPECT_EQ(queue.pop(), 42);

  producer.join();
}

// FIXME: exception safety checks: class that throws exception on copy or ctor

class _TMP__queue_mpmc
    : public ::testing::TestWithParam<
          std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> {};

TEST_P(_TMP__queue_mpmc, mpmc) {
  const auto& [items_size, queue_size, producers, consumers] = GetParam();

  std::vector<int> items_to_push(items_size);
  std::iota(std::begin(items_to_push), std::end(items_to_push), 0);

  _TMP_::queue::mpmc_queue<int> queue(queue_size);

  std::atomic<std::size_t> pushed = 0;

  std::mutex mutex;
  std::vector<int> popped_items;
  popped_items.reserve(items_to_push.size());

  std::latch latch(producers + consumers);
  std::vector<std::thread> threads;
  threads.reserve(producers + consumers);

  for (std::size_t i = 0; i < producers; ++i) {
    threads.emplace_back([&latch, &queue, &items_to_push, &pushed]() {
      latch.arrive_and_wait();
      while (true) {
        const std::size_t index =
            pushed.fetch_add(1, std::memory_order::relaxed);
        if (index >= items_to_push.size()) {
          return;
        }
        queue.push(items_to_push[index]);
      }
    });
  }

  for (std::size_t i = 0; i < consumers; ++i) {
    threads.emplace_back(
        [&latch, &queue, &items_to_push, &mutex, &popped_items]() {
          latch.arrive_and_wait();
          while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            if (auto item = queue.try_pop(); item.has_value()) {
              popped_items.push_back(std::move(item.value()));
            }
            if (popped_items.size() >= items_to_push.size()) {
              return;
            }
          }
        });
  }

  for (auto& thread : threads) {
    thread.join();
  }

  std::sort(std::begin(popped_items), std::end(popped_items));
  EXPECT_EQ(popped_items.size(), items_to_push.size());
  EXPECT_EQ(popped_items, items_to_push);
}

INSTANTIATE_TEST_SUITE_P(_TMP__queue, _TMP__queue_mpmc,
                         ::testing::Values(std::make_tuple(5, 3, 1, 1),
                                           std::make_tuple(100, 10, 4, 1),
                                           std::make_tuple(100, 10, 1, 4),
                                           std::make_tuple(10000, 100, 4, 4)));

}  // namespace tests::logging

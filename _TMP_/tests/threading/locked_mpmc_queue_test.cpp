#include "threading/locked_mpmc_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(_TMP__threading, mpmc_queue_size) {
  static_assert(sizeof(_TMP_::threading::locked_mpmc_queue<int, 10>) == 208);
  static_assert(alignof(_TMP_::threading::locked_mpmc_queue<int>) == 8);

  static_assert(sizeof(_TMP_::threading::locked_mpmc_queue<int>) == 216);
  static_assert(alignof(_TMP_::threading::locked_mpmc_queue<int>) == 8);
}

TEST(_TMP__threading, mpmc_queue_try_push_single_threaded) {
  _TMP_::threading::locked_mpmc_queue<int> queue(1);
  EXPECT_TRUE(queue.try_push(42));
  EXPECT_FALSE(queue.try_push(42));
}

TEST(_TMP__threading, mpmc_queue_try_pop_single_threaded) {
  _TMP_::threading::locked_mpmc_queue<int> queue(1);
  EXPECT_FALSE(queue.try_pop().has_value());

  queue.push(42);
  EXPECT_EQ(queue.try_pop(), 42);
  EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(_TMP__threading, mpmc_queue_size_capacity) {
  {
    _TMP_::threading::locked_mpmc_queue<std::string> queue(101);
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
    _TMP_::threading::locked_mpmc_queue<std::string, 101> queue;
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

TEST(_TMP__threading, mpmc_queue_blocking_push) {
  _TMP_::threading::locked_mpmc_queue<int> queue(2);

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

TEST(_TMP__threading, mpmc_queue_blocking_pop) {
  _TMP_::threading::locked_mpmc_queue<int> queue(1);

  std::thread producer([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    queue.push(42);
  });

  EXPECT_EQ(queue.pop(), 42);

  producer.join();
}

TEST(_TMP__threading, mpmc_queue_unblocking_try_pop) {
  _TMP_::threading::locked_mpmc_queue<int> queue(2);

  queue.push(1);
  queue.push(2);

  std::thread consumer([&queue]() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    EXPECT_EQ(queue.pop(), 1);
  });

  queue.push(3);
  EXPECT_EQ(queue.try_pop(), 2);

  consumer.join();
}

TEST(_TMP__threading, mpmc_queue_non_copyable_item_type) {
  struct non_copyable {
    constexpr non_copyable() noexcept = default;
    constexpr non_copyable(const non_copyable&) = delete;
    constexpr non_copyable& operator=(const non_copyable&) = delete;
    constexpr non_copyable(non_copyable&&) = default;
    constexpr non_copyable& operator=(non_copyable&&) = default;
  };

  _TMP_::threading::locked_mpmc_queue<non_copyable> queue(1);
  {
    queue.try_push(non_copyable{});
    [[maybe_unused]] const auto value = queue.try_pop();
  }
  {
    queue.push(non_copyable{});
    [[maybe_unused]] const auto value = queue.pop();
  }
}

TEST(_TMP__threading, mpmc_queue_throw_on_item_move) {
  static const std::runtime_error kException("exception");

  auto should_throw = std::make_shared<bool>(true);

  class throw_on_move_or_copy_if final {
   public:
    throw_on_move_or_copy_if(std::shared_ptr<bool> should_throw)
        : should_throw_(should_throw) {}
    throw_on_move_or_copy_if(throw_on_move_or_copy_if&& that)
        : should_throw_(that.should_throw_) {
      if (*should_throw_) {
        throw kException;
      }
    }
    throw_on_move_or_copy_if& operator=(throw_on_move_or_copy_if&& that) {
      should_throw_ = that.should_throw_;
      if (*should_throw_) {
        throw kException;
      }
      return *this;
    }
    throw_on_move_or_copy_if(const throw_on_move_or_copy_if& that)
        : should_throw_(that.should_throw_) {
      if (*should_throw_) {
        throw kException;
      }
    }
    throw_on_move_or_copy_if& operator=(const throw_on_move_or_copy_if& that) {
      should_throw_ = that.should_throw_;
      if (*should_throw_) {
        throw kException;
      }
      return *this;
    }

   private:
    std::shared_ptr<bool> should_throw_;
  };

  _TMP_::threading::locked_mpmc_queue<throw_on_move_or_copy_if> queue(1);

  {
    try {
      queue.push(throw_on_move_or_copy_if(should_throw));
      GTEST_FAIL();
    } catch (const std::runtime_error& e) {
      EXPECT_EQ(e.what(), kException.what());
    }
    EXPECT_EQ(queue.size(), 0);
    try {
      queue.try_push(throw_on_move_or_copy_if(should_throw));
      GTEST_FAIL();
    } catch (const std::runtime_error& e) {
      EXPECT_EQ(e.what(), kException.what());
    }
    EXPECT_EQ(queue.size(), 0);
  }
  {
    *should_throw = false;
    queue.push(throw_on_move_or_copy_if(should_throw));
    EXPECT_EQ(queue.size(), 1);
    *should_throw = true;
  }
  {
    try {
      queue.try_pop();
      GTEST_FAIL();
    } catch (const std::runtime_error& e) {
      EXPECT_EQ(e.what(), kException.what());
    }
    EXPECT_EQ(queue.size(), 1);

    try {
      queue.pop();
      GTEST_FAIL();
    } catch (const std::runtime_error& e) {
      EXPECT_EQ(e.what(), kException.what());
    }
    EXPECT_EQ(queue.size(), 1);
  }
}

class _TMP__threading
    : public ::testing::TestWithParam<
          std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> {};

TEST_P(_TMP__threading, mpmc_queue_mpmc_threads) {
  const auto& [items_size, queue_size, producers, consumers] = GetParam();

  std::vector<int> items_to_push(items_size);
  std::iota(items_to_push.begin(), items_to_push.end(), 0);

  _TMP_::threading::locked_mpmc_queue<int> queue(queue_size);

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

INSTANTIATE_TEST_SUITE_P(_TMP__threading, _TMP__threading,
                         ::testing::Values(std::make_tuple(5, 3, 1, 1),
                                           std::make_tuple(100, 10, 4, 1),
                                           std::make_tuple(100, 10, 1, 4),
                                           std::make_tuple(10000, 100, 4, 4)));

}  // namespace tests::threading

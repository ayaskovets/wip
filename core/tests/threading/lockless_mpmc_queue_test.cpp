#include "threading/lockless_mpmc_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <format>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(threading_lockless_mpmc_queue, size) {
  static_assert(sizeof(core::threading::lockless_mpmc_queue<int, 10>) == 192);
  static_assert(alignof(core::threading::lockless_mpmc_queue<int, 10>) == 64);

  static_assert(sizeof(core::threading::lockless_mpmc_queue<int>) == 192);
  static_assert(alignof(core::threading::lockless_mpmc_queue<int>) == 64);
}

TEST(threading_lockless_mpmc_queue, capacity) {
  EXPECT_EQ(core::threading::lockless_mpmc_queue<std::string>(128).capacity(),
            128);
  EXPECT_EQ(core::threading::lockless_mpmc_queue<std::string>(64).capacity(),
            64);
}

TEST(threading_lockless_mpmc_queue, smoke) {
  core::threading::lockless_mpmc_queue<int> queue(2);

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

TEST(threading_lockless_mpmc_queue, capacity_one) {
  EXPECT_ANY_THROW(core::threading::lockless_mpmc_queue<int>(1));
}

TEST(threading_lockless_mpmc_queue, shared_ptr) {
  auto ptr = std::make_shared<int>(42);

  core::threading::lockless_mpmc_queue<std::shared_ptr<int>> queue(2);
  EXPECT_EQ(ptr.use_count(), 1);
  EXPECT_TRUE(queue.try_push(ptr));
  EXPECT_EQ(ptr.use_count(), 2);
  EXPECT_TRUE(queue.try_push(ptr));
  EXPECT_EQ(ptr.use_count(), 3);
  EXPECT_TRUE(queue.try_pop().has_value());
  EXPECT_EQ(ptr.use_count(), 2);
  EXPECT_TRUE(queue.try_push(ptr));
  EXPECT_EQ(ptr.use_count(), 3);
  EXPECT_TRUE(queue.try_pop().has_value());
  EXPECT_EQ(ptr.use_count(), 2);
  EXPECT_TRUE(queue.try_pop().has_value());
  EXPECT_EQ(ptr.use_count(), 1);
}

TEST(threading_lockless_mpmc_queue, allocator) {
  struct lockless_mpmc_queue_entry final {
    int value;
    std::atomic<std::size_t> seqnum;

    constexpr int& get_value() { return value; }
    constexpr std::atomic<std::size_t>& get_seqnum() { return seqnum; }
  };

  std::unordered_map<lockless_mpmc_queue_entry*, std::size_t> allocations;

  class allocator : public std::allocator<lockless_mpmc_queue_entry> {
   public:
    allocator(std::unordered_map<lockless_mpmc_queue_entry*, std::size_t>&
                  allocations)
        : allocations_(allocations) {}

    lockless_mpmc_queue_entry* allocate(std::size_t n) {
      lockless_mpmc_queue_entry* ptr = new lockless_mpmc_queue_entry[n];
      allocations_[ptr] = n;
      return ptr;
    }
    void deallocate(lockless_mpmc_queue_entry* ptr, std::size_t n) {
      if ((allocations_[ptr] -= n) == 0) {
        allocations_.erase(ptr);
      }
      operator delete[](ptr, n);
    }

   private:
    std::unordered_map<lockless_mpmc_queue_entry*, std::size_t>& allocations_;
  };

  {
    core::threading::lockless_mpmc_queue<int, 2, allocator> queue(
        (allocator(allocations)));

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
  EXPECT_TRUE(allocations.empty());
}

TEST(threading_lockless_mpmc_queue, rollover) {
  core::threading::lockless_mpmc_queue<int> queue(8);

  for (std::size_t i = 0; i < 100; ++i) {
    EXPECT_TRUE(queue.try_push(i));
    EXPECT_EQ(queue.try_pop(), i);
  }
}

TEST(threading_lockless_mpmc_queue, non_copyable_item_type) {
  core::threading::lockless_mpmc_queue<std::unique_ptr<int>, 2> queue;
  queue.try_push(std::unique_ptr<int>{});
  [[maybe_unused]] const auto value = queue.try_pop();
}

TEST(threading_lockless_mpmc_queue, item_destructor) {
  struct non_copyable_counter {
    constexpr non_copyable_counter(std::size_t& constructed,
                                   std::size_t& move_constructed,
                                   std::size_t& destructed) noexcept
        : constructed(constructed),
          move_constructed(move_constructed),
          destructed(destructed) {
      ++constructed;
    }
    constexpr non_copyable_counter(non_copyable_counter&& that) noexcept
        : non_copyable_counter(that.constructed, that.move_constructed,
                               that.destructed) {
      move_constructed += 1;
    }
    constexpr non_copyable_counter(const non_copyable_counter&) = delete;
    constexpr non_copyable_counter& operator=(const non_copyable_counter&) =
        delete;
    constexpr non_copyable_counter& operator=(non_copyable_counter&&) = delete;
    constexpr ~non_copyable_counter() noexcept { ++destructed; }

    std::size_t& constructed;
    std::size_t& move_constructed;
    std::size_t& destructed;
  };

  {
    std::size_t constructed = 0;
    std::size_t move_constructed = 0;
    std::size_t destructed = 0;

    core::threading::lockless_mpmc_queue<non_copyable_counter> queue(2);

    non_copyable_counter pushed_item(constructed, move_constructed, destructed);

    EXPECT_EQ(constructed, 1);
    EXPECT_EQ(move_constructed, 0);
    EXPECT_EQ(destructed, 0);

    EXPECT_TRUE(queue.try_push(std::move(pushed_item)));

    EXPECT_EQ(constructed, 3);
    EXPECT_EQ(move_constructed, 2);
    EXPECT_EQ(destructed, 1);

    EXPECT_TRUE(queue.try_pop());

    EXPECT_EQ(constructed, 4);
    EXPECT_EQ(move_constructed, 3);
    EXPECT_EQ(destructed, 3);
  }
  {
    std::size_t constructed = 0;
    std::size_t move_constructed = 0;
    std::size_t destructed = 0;
    {
      core::threading::lockless_mpmc_queue<non_copyable_counter> queue(2);
      non_copyable_counter pushed_item(constructed, move_constructed,
                                       destructed);
      EXPECT_TRUE(queue.try_push(std::move(pushed_item)));
    }
    EXPECT_EQ(constructed, 4);
    EXPECT_EQ(move_constructed, 3);
    EXPECT_EQ(destructed, 4);
  }
}

class threading_lockless_mpmc_queue
    : public ::testing::TestWithParam<
          std::tuple<std::size_t, std::size_t, std::size_t, std::size_t>> {};

TEST_P(threading_lockless_mpmc_queue, workload) {
  const auto& [items_size, queue_size, producers, consumers] = GetParam();

  core::threading::lockless_mpmc_queue<int> queue(queue_size);

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
      while (!queue.try_push(items_to_push[index])) {
      }
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
      std::optional<int> popped_item;
      while (!(popped_item = queue.try_pop()).has_value()) {
      }
      popped_items[index] = *popped_item;
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
    threading_lockless_mpmc_queue, threading_lockless_mpmc_queue,
    ::testing::Values(std::make_tuple(5, 4, 1, 1),
                      std::make_tuple(100, 16, 4, 1),
                      std::make_tuple(100, 16, 1, 4),
                      std::make_tuple(30, 8, 4, 4),
                      std::make_tuple(10000, 128, 4, 4)),
    ([](const auto& info) {
      const auto& [items_size, queue_size, producers, consumers] = info.param;
      return std::format(
          "{}_items_size__{}_queue_size__{}_producers__{}_consumers",
          items_size, queue_size, producers, consumers);
    }));

}  // namespace tests::threading

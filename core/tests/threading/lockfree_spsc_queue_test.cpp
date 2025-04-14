#include "threading/lockfree_spsc_queue.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <format>
#include <latch>
#include <numeric>
#include <thread>

namespace tests::threading {

TEST(threading_lockfree_spsc_queue, size) {
  static_assert(sizeof(core::threading::lockfree_spsc_queue<int, 128>) == 320);
  static_assert(alignof(core::threading::lockfree_spsc_queue<int, 128>) == 64);

  static_assert(sizeof(core::threading::lockfree_spsc_queue<int>) == 320);
  static_assert(alignof(core::threading::lockfree_spsc_queue<int>) == 64);
}

TEST(threading_lockfree_spsc_queue, capacity) {
  EXPECT_EQ(core::threading::lockfree_spsc_queue<std::string>(128).capacity(),
            128);
  EXPECT_EQ(core::threading::lockfree_spsc_queue<std::string>(64).capacity(),
            64);
}

TEST(threading_lockfree_spsc_queue, minimal_capacity) {
  EXPECT_ANY_THROW(core::threading::lockfree_spsc_queue<int> queue(0));
}

TEST(threading_lockfree_spsc_queue, smoke) {
  core::threading::lockfree_spsc_queue<int> queue(2);

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

TEST(threading_lockfree_spsc_queue, capacity_one) {
  core::threading::lockfree_spsc_queue<int> queue(1);

  EXPECT_FALSE(queue.try_pop().has_value());
  EXPECT_TRUE(queue.try_push(1));
  EXPECT_FALSE(queue.try_push(3));
  EXPECT_EQ(queue.try_pop(), 1);
  EXPECT_TRUE(queue.try_push(4));
  EXPECT_FALSE(queue.try_push(5));
  EXPECT_EQ(queue.try_pop(), 4);
  EXPECT_FALSE(queue.try_pop().has_value());
}

TEST(threading_lockfree_spsc_queue, shared_ptr) {
  auto ptr = std::make_shared<int>(42);

  core::threading::lockfree_spsc_queue<std::shared_ptr<int>> queue(2);
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

TEST(threading_lockfree_spsc_queue, allocator) {
  struct alignas(32) value_type final {
    explicit constexpr operator std::uint32_t&() { return value; }

    float prefix_padding;
    std::uint32_t value;
  };
  std::unordered_map<value_type*, std::size_t> allocations;

  {
    class allocator : public std::allocator<value_type> {
     public:
      explicit allocator(
          std::unordered_map<value_type*, std::size_t>& allocations)
          : allocations_(allocations) {}

      value_type* allocate(std::size_t n) {
        value_type* ptr = std::allocator<value_type>::allocate(n);

        ptr->prefix_padding = 42.f;

        allocations_[ptr] = n;
        return ptr;
      }
      void deallocate(value_type* ptr, std::size_t n) {
        EXPECT_EQ(ptr->prefix_padding, 42.f);

        if ((allocations_[ptr] -= n) == 0) {
          allocations_.erase(ptr);
        }
        std::allocator<value_type>::deallocate(ptr, n);
      }

     private:
      std::unordered_map<value_type*, std::size_t>& allocations_;
    };

    allocator alloc(allocations);
    core::threading::lockfree_spsc_queue<std::uint32_t, 2, allocator> queue(
        alloc);

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

TEST(threading_lockfree_spsc_queue, rollover) {
  core::threading::lockfree_spsc_queue<int> queue(5);

  for (std::size_t i = 0; i < 100; ++i) {
    EXPECT_TRUE(queue.try_push(i));
    EXPECT_EQ(queue.try_pop(), i);
  }
}

TEST(threading_lockfree_spsc_queue, non_copyable_item_type) {
  core::threading::lockfree_spsc_queue<std::unique_ptr<int>, 1> queue;
  queue.try_push(std::unique_ptr<int>{});
  [[maybe_unused]] const auto value = queue.try_pop();
}

TEST(threading_lockfree_spsc_queue, item_destructor) {
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

    core::threading::lockfree_spsc_queue<non_copyable_counter> queue(1);

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
      core::threading::lockfree_spsc_queue<non_copyable_counter> queue(1);
      non_copyable_counter pushed_item(constructed, move_constructed,
                                       destructed);
      EXPECT_TRUE(queue.try_push(std::move(pushed_item)));
    }
    EXPECT_EQ(constructed, 4);
    EXPECT_EQ(move_constructed, 3);
    EXPECT_EQ(destructed, 4);
  }
}

class threading_lockfree_spsc_queue
    : public ::testing::TestWithParam<std::tuple<std::size_t, std::size_t>> {};

TEST_P(threading_lockfree_spsc_queue, workload) {
  const auto& [items_size, queue_size] = GetParam();

  std::vector<int> items_to_push(items_size);
  std::iota(items_to_push.begin(), items_to_push.end(), 0);

  core::threading::lockfree_spsc_queue<int> queue(queue_size);

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
      if (const std::optional<int> item = queue.try_pop(); item.has_value()) {
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
    threading_lockfree_spsc_queue, threading_lockfree_spsc_queue,
    ::testing::Values(std::make_tuple(5, 3), std::make_tuple(8, 4),
                      std::make_tuple(5, 1), std::make_tuple(6, 2),
                      std::make_tuple(7, 2), std::make_tuple(10, 1),
                      std::make_tuple(20, 2), std::make_tuple(100, 10),
                      std::make_tuple(10000, 100)),
    ([](const auto& info) {
      const auto& [items_size, queue_size] = info.param;
      return std::format("{}_items_size__{}_queue_size", items_size,
                         queue_size);
    }));

}  // namespace tests::threading

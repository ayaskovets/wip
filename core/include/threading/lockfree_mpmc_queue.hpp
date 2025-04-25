#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"
#include "utils/predicates.hpp"

namespace core::threading {

namespace detail {

template <typename T, typename Value, typename Index>
concept lockfree_mpmc_queue_entry = requires(T entry) {
  requires std::atomic<T>::is_always_lock_free;
  { entry.get_value() } -> std::same_as<Value&>;
  { entry.get_seqnum() } -> std::same_as<Index&>;
};

}  // namespace detail

template <typename Value, typename Index>
struct lockfree_mpmc_queue_entry final {
  Value value;
  Index seqnum;

  constexpr Value& get_value() { return value; }
  constexpr Index& get_seqnum() { return seqnum; }
};

template <typename Value, std::unsigned_integral Index = std::size_t,
          Index Capacity = utils::kDynamicCapacity<Index>,
          typename Allocator =
              std::allocator<lockfree_mpmc_queue_entry<Value, Index>>>
class lockfree_mpmc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(
      std::is_trivially_destructible_v<typename Allocator::value_type>);
  static_assert(std::is_nothrow_destructible_v<typename Allocator::value_type>);
  static_assert(
      std::is_trivially_copy_constructible_v<typename Allocator::value_type>);
  static_assert(
      std::is_nothrow_copy_constructible_v<typename Allocator::value_type>);
  static_assert(
      std::is_trivially_move_assignable_v<typename Allocator::value_type>);
  static_assert(
      std::is_nothrow_move_assignable_v<typename Allocator::value_type>);
  static_assert(detail::lockfree_mpmc_queue_entry<
                typename Allocator::value_type, Value, Index>);

 public:
  using value_t = Value;
  using index_t = Index;
  using allocator_t = Allocator;
  using entry_t = typename Allocator::value_type;

 public:
  constexpr explicit lockfree_mpmc_queue(allocator_t allocator = allocator_t())
    requires(Capacity != utils::kDynamicCapacity<Index>)
      : allocator_(std::move(allocator)) {
    if (!(ring_buffer_ = allocator_.allocate(Capacity))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    std::memset(ring_buffer_, 0, *capacity_ * sizeof(entry_t));
    for (index_t i = 0; i < *capacity_; ++i) {
      ring_buffer_[i].get_seqnum() = (i << 1);
    }
  }

  constexpr explicit lockfree_mpmc_queue(index_t capacity,
                                         allocator_t allocator = allocator_t())
    requires(Capacity == utils::kDynamicCapacity<Index>)
      : allocator_(std::move(allocator)), capacity_(capacity) {
    if (!(utils::is_power_of_two(*capacity_) && *capacity_ >= 2)) {
      throw std::invalid_argument(
          "capacity must be a power of two greater than one");
    }

    if (!(ring_buffer_ = allocator_.allocate(*capacity_))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    std::memset(ring_buffer_, 0, *capacity_ * sizeof(entry_t));
    for (index_t i = 0; i < *capacity_; ++i) {
      ring_buffer_[i].get_seqnum() = (i << 1);
    }
  }

  constexpr ~lockfree_mpmc_queue() noexcept {
    value_t value;
    while (try_pop(value));
    allocator_.deallocate(ring_buffer_, *capacity_);
  }

 public:
  constexpr bool try_push(value_t value) noexcept {
    index_t push_to = push_to_.load(std::memory_order::relaxed);
    entry_t entry = reinterpret_cast<std::atomic<entry_t>&>(
                        ring_buffer_[push_to & (*capacity_ - 1)])
                        .load(std::memory_order::acquire);

    if (entry.get_seqnum() == (push_to << 1)) {
      entry_t desired;
      std::memset(&desired, 0, sizeof(entry_t));
      desired.get_value() = std::move(value);
      desired.get_seqnum() = ((push_to << 1) | 1);

      if (reinterpret_cast<std::atomic<entry_t>&>(
              ring_buffer_[push_to & (*capacity_ - 1)])
              .compare_exchange_strong(entry, desired,
                                       std::memory_order::release,
                                       std::memory_order::acquire)) {
        push_to_.compare_exchange_strong(push_to, push_to + 1,
                                         std::memory_order::release,
                                         std::memory_order::acquire);
        return true;
      }
    } else if (entry.get_seqnum() == ((push_to << 1) | 1) ||
               entry.get_seqnum() == ((push_to + *capacity_) << 1)) {
      push_to_.compare_exchange_strong(push_to, push_to + 1,
                                       std::memory_order::release,
                                       std::memory_order::acquire);
    }
    return false;
  }

  constexpr void push(value_t value) noexcept { while (!try_push(value)); }

  constexpr bool try_pop(value_t& value) noexcept {
    index_t pop_from = pop_from_.load(std::memory_order::relaxed);
    entry_t entry = reinterpret_cast<std::atomic<entry_t>&>(
                        ring_buffer_[pop_from & (*capacity_ - 1)])
                        .load(std::memory_order::acquire);

    if (entry.get_seqnum() == ((pop_from << 1) | 1)) {
      entry_t desired;
      std::memset(&desired, 0, sizeof(entry_t));
      desired.get_value() = value_t{};
      desired.get_seqnum() = ((pop_from + *capacity_) << 1);

      if (reinterpret_cast<std::atomic<entry_t>&>(
              ring_buffer_[pop_from & (*capacity_ - 1)])
              .compare_exchange_strong(entry, desired,
                                       std::memory_order::release,
                                       std::memory_order::acquire)) {
        value = std::move(entry.get_value());
        pop_from_.compare_exchange_strong(pop_from, pop_from + 1,
                                          std::memory_order::release,
                                          std::memory_order::acquire);
        return true;
      }
    } else if ((entry.get_seqnum() | 1) ==
               (((pop_from + *capacity_) << 1) | 1)) {
      pop_from_.compare_exchange_strong(pop_from, pop_from + 1,
                                        std::memory_order::release,
                                        std::memory_order::acquire);
    }
    return false;
  }

  constexpr value_t pop() noexcept {
    value_t value;
    while (!try_pop(value));
    return value;
  }

 public:
  constexpr index_t capacity() const noexcept { return *capacity_; }

 private:
  entry_t* ring_buffer_ = nullptr;

  alignas(utils::kCacheLineSize) std::atomic<index_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<index_t> pop_from_ = 0;

  [[no_unique_address]] allocator_t allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      index_t, Capacity == utils::kDynamicCapacity<Index>, Capacity> capacity_;
};

}  // namespace core::threading

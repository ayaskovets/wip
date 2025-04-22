#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"
#include "utils/predicates.hpp"

namespace core::threading {

// NOTE: this queue does not handle push & pop indices overflow well, but using
// 64-bit unsigned integers as indices makes the problem pretty much
// non-existent because even handling 1B messages per second overflow would
// happed once in 584 years
template <typename T, std::size_t Capacity = utils::kRuntimeCapacity,
          typename Allocator =
              std::allocator<std::pair<T, std::atomic<std::size_t>>>>
class lockless_mpmc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(std::is_nothrow_destructible_v<T>,
                "T is required to be nothrow move destructible to remove "
                "copies in push() and try_push()");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T is required to be nothrow move destructible to remove "
                "copies in pop()");
  static_assert(std::is_nothrow_move_assignable_v<T>,
                "T is required to be nothrow move assignable to remove "
                "copies in try_pop()");
  static_assert(std::atomic<std::size_t>::is_always_lock_free,
                "std::atomic<std::size_t> is required to be lockfree for queue "
                "to function efficiently");
  static constexpr void static_assert_allocator_value_type() {
    typename Allocator::value_type value_type;
    auto&& [item, seqnum] = value_type;
    static_assert(std::is_same_v<decltype(item), T>,
                  "allocator value_type must be decomposable into [T, "
                  "std::atomic<std::size_t>]");
    static_assert(std::is_same_v<decltype(seqnum), std::atomic<std::size_t>>,
                  "allocator value_type must be decomposable into [T, "
                  "std::atomic<std::size_t>]");
  }

 public:
  constexpr explicit lockless_mpmc_queue(
      const Allocator& allocator = Allocator())
    requires(Capacity != utils::kRuntimeCapacity &&
             utils::is_power_of_two(Capacity) && Capacity > 1)
      : allocator_(allocator) {
    static_assert_allocator_value_type();

    if (!(ring_buffer_ = allocator_.allocate(Capacity))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    for (std::size_t i = 0; i < *capacity_; ++i) {
      auto& [item, seqnum] = ring_buffer_[i];
      seqnum.store(i, std::memory_order::relaxed);
    }
  }

  constexpr explicit lockless_mpmc_queue(
      std::size_t capacity, const Allocator& allocator = Allocator())
    requires(Capacity == utils::kRuntimeCapacity)
      : allocator_(allocator), capacity_(capacity) {
    static_assert_allocator_value_type();

    if (!utils::is_power_of_two(*capacity_) || *capacity_ <= 1) {
      throw std::invalid_argument(
          "capacity must be a power of two greater than one");
    }

    if (!(ring_buffer_ = allocator_.allocate(*capacity_))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    for (std::size_t i = 0; i < *capacity_; ++i) {
      auto& [item, seqnum] = ring_buffer_[i];
      seqnum.store(i, std::memory_order::relaxed);
    }
  }

  constexpr ~lockless_mpmc_queue() noexcept {
    std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
    while (true) {
      auto& [item, seqnum] = ring_buffer_[pop_from & (*capacity_ - 1)];
      if (pop_from + 1 != seqnum.load(std::memory_order::acquire)) {
        break;
      }
      std::destroy_at(&item);
      seqnum.store(pop_from + *capacity_, std::memory_order::release);
      pop_from += 1;
    }
    allocator_.deallocate(ring_buffer_, *capacity_);
  }

 public:
  constexpr bool try_push(T value) noexcept {
    std::size_t push_to = push_to_.load(std::memory_order::relaxed);
    auto& [item, seqnum] = ring_buffer_[push_to & (*capacity_ - 1)];

    if (push_to != seqnum.load(std::memory_order::acquire)) {
      return false;
    }
    if (!push_to_.compare_exchange_weak(push_to, push_to + 1,
                                        std::memory_order::relaxed)) {
      return false;
    }

    std::construct_at(&item, std::move(value));
    seqnum.store(push_to + 1, std::memory_order::release);
    return true;
  }

  constexpr void push(T value) noexcept {
    do {
      std::size_t push_to = push_to_.load(std::memory_order::relaxed);
      auto& [item, seqnum] = ring_buffer_[push_to & (*capacity_ - 1)];
      if (push_to != seqnum.load(std::memory_order::acquire)) {
        continue;
      }

      if (push_to_.compare_exchange_weak(push_to, push_to + 1,
                                         std::memory_order::relaxed)) {
        std::construct_at(&item, std::move(value));
        seqnum.store(push_to + 1, std::memory_order::release);
        break;
      }
    } while (true);
  }

  constexpr bool try_pop(T& value) noexcept {
    std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
    auto& [item, seqnum] = ring_buffer_[pop_from & (*capacity_ - 1)];

    if (pop_from + 1 != seqnum.load(std::memory_order::acquire)) {
      return false;
    }
    if (!pop_from_.compare_exchange_weak(pop_from, pop_from + 1,
                                         std::memory_order::relaxed)) {
      return false;
    }

    value = std::move(item);
    std::destroy_at(&item);
    seqnum.store(pop_from + *capacity_, std::memory_order::release);
    return true;
  }

  constexpr T pop() noexcept {
    do {
      std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
      auto& [item, seqnum] = ring_buffer_[pop_from & (*capacity_ - 1)];
      if (pop_from + 1 != seqnum.load(std::memory_order::acquire)) {
        continue;
      }

      if (pop_from_.compare_exchange_weak(pop_from, pop_from + 1,
                                          std::memory_order::relaxed)) {
        T value(std::move(item));
        std::destroy_at(&item);
        seqnum.store(pop_from + *capacity_, std::memory_order::release);
        return value;
      }
    } while (true);
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  typename Allocator::value_type* ring_buffer_ = nullptr;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;

  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == utils::kRuntimeCapacity, Capacity> capacity_;
};

}  // namespace core::threading

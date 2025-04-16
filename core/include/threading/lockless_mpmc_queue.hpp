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
                "copies in push()");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T is required to be nothrow move destructible to remove "
                "copies in pop()");
  static_assert(std::atomic<std::size_t>::is_always_lock_free,
                "std::atomic<std::size_t> is required to be lockfree for queue "
                "to function efficiently");

 public:
  constexpr explicit lockless_mpmc_queue(
      const Allocator& allocator = Allocator())
    requires(Capacity != utils::kRuntimeCapacity &&
             utils::is_power_of_two(Capacity) && Capacity > 1)
      : allocator_(allocator) {
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
    while (try_pop().has_value()) {
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

  constexpr std::optional<T> try_pop() noexcept {
    std::optional<T> value;

    std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
    auto& [item, seqnum] = ring_buffer_[pop_from & (*capacity_ - 1)];

    if (pop_from + 1 != seqnum.load(std::memory_order::acquire)) {
      return value;
    }

    if (!pop_from_.compare_exchange_weak(pop_from, pop_from + 1,
                                         std::memory_order::relaxed)) {
      return value;
    }

    value.emplace(std::move(item));
    std::destroy_at(&item);
    seqnum.store(pop_from + *capacity_, std::memory_order::release);
    return value;
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

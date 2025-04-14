#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"

namespace core::threading {

// NOTE: this queue contains an additional fake element to allow for an
// efficient implementation of a ring buffer indexing, so the allocated buffer
// is always equal to the provided capacity + 1
template <typename T, std::size_t Capacity = utils::kRuntimeCapacity,
          typename Allocator = std::allocator<T>>
class lockfree_spsc_queue final : utils::non_copyable, utils::non_movable {
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
  constexpr explicit lockfree_spsc_queue(
      const Allocator& allocator = Allocator())
    requires(Capacity != utils::kRuntimeCapacity && Capacity > 0)
      : allocator_(allocator) {
    if (!(ring_buffer_ = allocator_.allocate(*capacity_ + 1))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr explicit lockfree_spsc_queue(
      std::size_t capacity, const Allocator& allocator = Allocator())
    requires(Capacity == utils::kRuntimeCapacity)
      : allocator_(allocator), capacity_(capacity) {
    if (*capacity_ <= 0) [[unlikely]] {
      throw std::invalid_argument("capacity must be greater than zero");
    }

    if (!(ring_buffer_ = allocator_.allocate(*capacity_ + 1))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr ~lockfree_spsc_queue() noexcept {
    while (try_pop().has_value()) {
    }
    allocator_.deallocate(ring_buffer_, *capacity_ + 1);
  }

 public:
  constexpr bool try_push(T value) noexcept {
    const std::size_t push_to = push_to_.load(std::memory_order::relaxed);
    const std::size_t next_push_to = (push_to == *capacity_) ? 0 : push_to + 1;

    if (next_push_to == cached_pop_from_ &&
        (next_push_to ==
         (cached_pop_from_ = pop_from_.load(std::memory_order::acquire)))) {
      return false;
    }

    std::construct_at(&reinterpret_cast<T&>(ring_buffer_[push_to]),
                      std::move(value));
    push_to_.store(next_push_to, std::memory_order::release);
    return true;
  }

  constexpr std::optional<T> try_pop() noexcept {
    std::optional<T> value;

    const std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
    if (pop_from == cached_push_to_ &&
        (pop_from ==
         (cached_push_to_ = push_to_.load(std::memory_order::acquire)))) {
      return value;
    }

    value.emplace(std::move(reinterpret_cast<T&>(ring_buffer_[pop_from])));

    std::destroy_at(&reinterpret_cast<T&>(ring_buffer_[pop_from]));
    const std::size_t next_pop_from =
        (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return value;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  typename Allocator::value_type* ring_buffer_ = nullptr;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_pop_from_ = 0;

  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == utils::kRuntimeCapacity, Capacity> capacity_;
};

}  // namespace core::threading

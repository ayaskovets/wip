#pragma once

#include <atomic>
#include <optional>
#include <span>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"

namespace core::threading {

namespace detail {

template <typename T>
struct alignas(utils::kCacheLineSize) aligned_to_cache_line final {
  T value;
};

}  // namespace detail

template <typename T, std::size_t Capacity = std::dynamic_extent,
          typename Allocator = std::allocator<detail::aligned_to_cache_line<T>>>
  requires(std::is_nothrow_destructible_v<T> &&
           std::is_nothrow_move_constructible_v<T>)
class lockfree_spsc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(sizeof(typename Allocator::value_type) <=
                utils::kCacheLineSize);
  static_assert(alignof(typename Allocator::value_type) <=
                utils::kCacheLineSize);

 public:
  constexpr lockfree_spsc_queue(const Allocator& allocator = Allocator())
    requires(Capacity != std::dynamic_extent)
      : ring_buffer_(nullptr), allocator_(allocator) {
    if (!(ring_buffer_ = allocator_.allocate(Capacity + 1))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr explicit lockfree_spsc_queue(
      std::size_t capacity, const Allocator& allocator = Allocator())
    requires(Capacity == std::dynamic_extent)
      : ring_buffer_(nullptr), allocator_(allocator), capacity_(capacity) {
    if (!(ring_buffer_ = allocator_.allocate(capacity + 1))) [[unlikely]] {
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
    const auto push_to = push_to_.load(std::memory_order::relaxed);
    const auto next_push_to = (push_to == *capacity_) ? 0 : push_to + 1;

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

    const auto pop_from = pop_from_.load(std::memory_order::relaxed);
    if (pop_from == cached_push_to_ &&
        (pop_from ==
         (cached_push_to_ = push_to_.load(std::memory_order::acquire)))) {
      return value;
    }

    value.emplace(std::move(reinterpret_cast<T&>(ring_buffer_[pop_from])));

    std::destroy_at(&ring_buffer_[pop_from]);
    const auto next_pop_from = (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return value;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  typename Allocator::value_type* ring_buffer_;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_pop_from_ = 0;

  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;
};

}  // namespace core::threading

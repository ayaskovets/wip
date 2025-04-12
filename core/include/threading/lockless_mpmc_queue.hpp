#pragma once

#include <atomic>
#include <concepts>
#include <optional>
#include <span>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"
#include "utils/predicates.hpp"

namespace core::threading {

namespace detail {

template <typename Allocator, typename T>
concept lockless_mpmc_queue_entry_concept =
    requires(typename Allocator::value_type allocator_value_type) {
      { allocator_value_type.get_value() } -> std::same_as<T&>;
      {
        allocator_value_type.get_seqnum()
      } -> std::same_as<std::atomic<std::size_t>&>;
    };

template <typename T>
struct alignas(utils::kCacheLineSize)
    lockless_mpmc_queue_default_aligned_entry final {
 public:
  T value;
  std::atomic<std::size_t> seqnum;

 public:
  constexpr T& get_value() noexcept { return value; }
  constexpr std::atomic<std::size_t>& get_seqnum() noexcept { return seqnum; }
};

}  // namespace detail

template <typename T, std::size_t Capacity = std::dynamic_extent,
          typename Allocator = std::allocator<
              detail::lockless_mpmc_queue_default_aligned_entry<T>>>
  requires(std::is_nothrow_destructible_v<T> &&
           std::is_nothrow_move_constructible_v<T>)
class lockless_mpmc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(sizeof(typename Allocator::value_type) <=
                utils::kCacheLineSize);
  static_assert(alignof(typename Allocator::value_type) <=
                utils::kCacheLineSize);
  static_assert(std::atomic<std::size_t>::is_always_lock_free);
  static_assert(detail::lockless_mpmc_queue_entry_concept<Allocator, T>);

 public:
  constexpr lockless_mpmc_queue(const Allocator& allocator = Allocator())
    requires(Capacity != std::dynamic_extent &&
             utils::is_power_of_two(Capacity) && Capacity > 1)
      : ring_buffer_(nullptr), allocator_(allocator) {
    if (!(ring_buffer_ = allocator_.allocate(Capacity))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    for (std::size_t i = 0; i < *capacity_; ++i) {
      ring_buffer_[i].get_seqnum().store(i, std::memory_order::relaxed);
    }
  }

  constexpr explicit lockless_mpmc_queue(
      std::size_t capacity, const Allocator& allocator = Allocator())
    requires(Capacity == std::dynamic_extent)
      : ring_buffer_(nullptr), allocator_(allocator), capacity_(capacity) {
    if (!utils::is_power_of_two(capacity) || capacity <= 1) {
      throw std::invalid_argument(
          "capacity must be a power of two greater than one");
    }

    if (!(ring_buffer_ = allocator_.allocate(capacity))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    for (std::size_t i = 0; i < *capacity_; ++i) {
      ring_buffer_[i].get_seqnum().store(i, std::memory_order::relaxed);
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
    typename Allocator::value_type& entry =
        ring_buffer_[push_to & (*capacity_ - static_cast<std::size_t>(1))];
    const std::size_t seqnum =
        entry.get_seqnum().load(std::memory_order::acquire);
    if (seqnum == push_to) {
      if (push_to_.compare_exchange_weak(push_to,
                                         push_to + static_cast<std::size_t>(1),
                                         std::memory_order::relaxed)) {
        std::construct_at(&entry.get_value(), std::move(value));
        entry.get_seqnum().store(push_to + static_cast<std::size_t>(1),
                                 std::memory_order::release);
        return true;
      }
    }
    return false;
  }

  constexpr std::optional<T> try_pop() noexcept {
    std::optional<T> value;
    std::size_t pop_from = pop_from_.load(std::memory_order::relaxed);
    typename Allocator::value_type& entry =
        ring_buffer_[pop_from & (*capacity_ - static_cast<std::size_t>(1))];
    const std::size_t seqnum =
        entry.get_seqnum().load(std::memory_order::acquire);
    if (seqnum ==
        static_cast<std::size_t>(pop_from + static_cast<std::size_t>(1))) {
      if (pop_from_.compare_exchange_weak(
              pop_from, pop_from + static_cast<std::size_t>(1),
              std::memory_order::relaxed)) {
        value.emplace(std::move(entry.get_value()));
        std::destroy_at(&entry.get_value());
        entry.get_seqnum().store(pop_from + *capacity_,
                                 std::memory_order::release);
        return value;
      }
    }
    return value;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  typename Allocator::value_type* ring_buffer_;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;

  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;
};

}  // namespace core::threading

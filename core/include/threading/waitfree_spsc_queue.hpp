#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"

namespace core::threading {

namespace detail {

template <typename T, typename Value>
concept waitfree_spsc_queue_entry = requires(T entry) {
  { entry.value() } -> std::same_as<Value&>;
};

}  // namespace detail

template <typename Value>
class alignas(Value) waitfree_spsc_queue_entry final {
 private:
  Value value_;

 public:
  constexpr Value& value() { return value_; }
};

// NOTE: this queue contains one additional fake element to allow for an
// efficient implementation of a ring buffer indexing, so the allocated buffer
// is always equal to the provided capacity + 1
template <typename Value, std::unsigned_integral Index = std::size_t,
          Index Capacity = utils::kDynamicCapacity<Index>,
          typename Allocator = std::allocator<waitfree_spsc_queue_entry<Value>>>
class waitfree_spsc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(std::is_nothrow_default_constructible_v<Value>);
  static_assert(std::is_nothrow_destructible_v<Value>);
  static_assert(std::is_nothrow_move_constructible_v<Value>);
  static_assert(std::is_nothrow_move_assignable_v<Value>);
  static_assert(
      detail::waitfree_spsc_queue_entry<typename Allocator::value_type, Value>);

 public:
  using value_t = Value;
  using index_t = Index;
  using allocator_t = Allocator;
  using entry_t = typename Allocator::value_type;

 public:
  constexpr explicit waitfree_spsc_queue(
      const allocator_t& allocator = allocator_t())
    requires(Capacity != utils::kDynamicCapacity<index_t> && Capacity > 0)
      : allocator_(allocator) {
    if (!(ring_buffer_ = allocator_.allocate(*capacity_ + 1))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr explicit waitfree_spsc_queue(
      index_t capacity, const allocator_t& allocator = allocator_t())
    requires(Capacity == utils::kDynamicCapacity<index_t>)
      : allocator_(allocator), capacity_(capacity) {
    if (*capacity_ <= 0) [[unlikely]] {
      throw std::invalid_argument("capacity must be greater than zero");
    }

    if (!(ring_buffer_ = allocator_.allocate(*capacity_ + 1))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr ~waitfree_spsc_queue() noexcept {
    value_t value;
    while (try_pop(value));
    allocator_.deallocate(ring_buffer_, *capacity_ + 1);
  }

 public:
  constexpr bool try_push(value_t value) noexcept {
    const index_t push_to = push_to_.load(std::memory_order::relaxed);
    const index_t next_push_to = (push_to == *capacity_) ? 0 : push_to + 1;

    if (next_push_to == cached_pop_from_ &&
        (next_push_to ==
         (cached_pop_from_ = pop_from_.load(std::memory_order::acquire)))) {
      return false;
    }

    std::construct_at(&ring_buffer_[push_to].value(), std::move(value));

    push_to_.store(next_push_to, std::memory_order::release);
    return true;
  }

  constexpr void push(value_t value) noexcept {
    const index_t push_to = push_to_.load(std::memory_order::relaxed);
    const index_t next_push_to = (push_to == *capacity_) ? 0 : push_to + 1;

    while (next_push_to == cached_pop_from_ &&
           (next_push_to ==
            (cached_pop_from_ = pop_from_.load(std::memory_order::acquire))));

    std::construct_at(&ring_buffer_[push_to].value(), std::move(value));

    push_to_.store(next_push_to, std::memory_order::release);
  }

  constexpr bool try_pop(value_t& value) noexcept {
    const index_t pop_from = pop_from_.load(std::memory_order::relaxed);
    if (pop_from == cached_push_to_ &&
        (pop_from ==
         (cached_push_to_ = push_to_.load(std::memory_order::acquire)))) {
      return false;
    }

    value = std::move(ring_buffer_[pop_from].value());
    std::destroy_at(&ring_buffer_[pop_from].value());

    const index_t next_pop_from = (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return true;
  }

  constexpr value_t pop() noexcept {
    const index_t pop_from = pop_from_.load(std::memory_order::relaxed);
    while (pop_from == cached_push_to_ &&
           (pop_from ==
            (cached_push_to_ = push_to_.load(std::memory_order::acquire))));

    value_t value(std::move(ring_buffer_[pop_from].value()));
    std::destroy_at(&ring_buffer_[pop_from].value());

    const index_t next_pop_from = (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return value;
  }

 public:
  constexpr index_t capacity() const noexcept { return *capacity_; }

 private:
  entry_t* ring_buffer_ = nullptr;

  alignas(utils::kCacheLineSize) std::atomic<index_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) index_t cached_push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<index_t> pop_from_ = 0;
  alignas(utils::kCacheLineSize) index_t cached_pop_from_ = 0;

  [[no_unique_address]] allocator_t allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      index_t, Capacity == utils::kDynamicCapacity<index_t>, Capacity>
      capacity_;
};

}  // namespace core::threading

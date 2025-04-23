#pragma once

#include <atomic>
#include <optional>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"
#include "utils/predicates.hpp"

namespace core::threading {

template <typename T, std::size_t Capacity = utils::kRuntimeCapacity,
          typename Allocator = std::allocator<std::pair<T, std::atomic<bool>>>>
class lockfree_mpsc_queue final : utils::non_copyable, utils::non_movable {
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
  static_assert(std::atomic<bool>::is_always_lock_free,
                "std::atomic<bool> is required to be lockfree for queue "
                "to function efficiently");
  static constexpr void static_assert_allocator_value_type() {
    typename Allocator::value_type value_type;
    auto&& [item, is_occupied] = value_type;
    static_assert(std::is_same_v<decltype(item), T>,
                  "allocator value_type must be decomposable into [T, "
                  "std::atomic<bool>]");
    static_assert(std::is_same_v<decltype(is_occupied), std::atomic<bool>>,
                  "allocator value_type must be decomposable into [T, "
                  "std::atomic<bool>]");
  }

 public:
  constexpr explicit lockfree_mpsc_queue(
      const Allocator& allocator = Allocator())
    requires(Capacity != utils::kRuntimeCapacity &&
             utils::is_power_of_two(Capacity) && Capacity > 1)
      : allocator_(allocator) {
    static_assert_allocator_value_type();

    if (!(ring_buffer_ = allocator_.allocate(Capacity))) [[unlikely]] {
      throw std::runtime_error("failed to allocate memory");
    }

    for (std::size_t i = 0; i < *capacity_; ++i) {
      auto& [item, is_occupied] = ring_buffer_[i];
      is_occupied.store(false, std::memory_order::relaxed);
    }
  }

  constexpr explicit lockfree_mpsc_queue(
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
      auto& [item, is_occupied] = ring_buffer_[i];
      is_occupied.store(false, std::memory_order::relaxed);
    }
  }

  constexpr ~lockfree_mpsc_queue() noexcept {
    std::size_t pop_from = pop_from_;
    while (true) {
      auto& [item, is_occupied] = ring_buffer_[pop_from & (*capacity_ - 1)];
      if (!is_occupied.load(std::memory_order::acquire)) {
        break;
      }
      std::destroy_at(&item);
      is_occupied.store(false, std::memory_order::release);
      pop_from += 1;
    }
    allocator_.deallocate(ring_buffer_, *capacity_);
  }

 public:
  constexpr bool try_push(T value) noexcept {
    const std::size_t size = size_.fetch_add(1, std::memory_order::acquire);
    if (size >= *capacity_) {
      size_.fetch_sub(1, std::memory_order::release);
      return false;
    }

    const std::size_t push_to =
        push_to_.fetch_add(1, std::memory_order::relaxed);
    auto& [item, is_occupied] = ring_buffer_[push_to & (*capacity_ - 1)];

    std::construct_at(&reinterpret_cast<T&>(item), std::move(value));

    is_occupied.store(true, std::memory_order::release);
    is_occupied.notify_one();
    return true;
  }

  constexpr void push(T value) noexcept {
    const std::size_t size = size_.fetch_add(1, std::memory_order::acquire);
    if (size >= *capacity_) {
      std::size_t actual_size = size;
      do {
        size_.wait(actual_size, std::memory_order::acquire);
      } while ((actual_size = size_.load(std::memory_order::acquire)) >=
               size + 1);
    }

    const std::size_t push_to =
        push_to_.fetch_add(1, std::memory_order::relaxed);
    auto& [item, is_occupied] = ring_buffer_[push_to & (*capacity_ - 1)];

    std::construct_at(&reinterpret_cast<T&>(item), std::move(value));

    is_occupied.store(true, std::memory_order::release);
    is_occupied.notify_one();
    return;
  }

  constexpr bool try_pop(T& value) noexcept {
    auto& [item, is_occupied] = ring_buffer_[pop_from_ & (*capacity_ - 1)];
    if (!is_occupied.load(std::memory_order::acquire)) {
      return false;
    }

    value = std::move(reinterpret_cast<T&>(item));
    std::destroy_at(&reinterpret_cast<T&>(item));

    pop_from_ += 1;
    is_occupied.store(false, std::memory_order::relaxed);
    size_.fetch_sub(1, std::memory_order::release);
    size_.notify_one();
    return true;
  }

  constexpr T pop() noexcept {
    auto& [item, is_occupied] = ring_buffer_[pop_from_ & (*capacity_ - 1)];
    is_occupied.wait(false, std::memory_order::acquire);

    T value(std::move(reinterpret_cast<T&>(item)));
    std::destroy_at(&reinterpret_cast<T&>(item));

    pop_from_ += 1;
    is_occupied.store(false, std::memory_order::relaxed);
    size_.fetch_sub(1, std::memory_order::release);
    size_.notify_one();
    return value;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  typename Allocator::value_type* ring_buffer_ = nullptr;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> size_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::size_t pop_from_ = 0;

  [[no_unique_address]] Allocator allocator_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == utils::kRuntimeCapacity, Capacity> capacity_;
};

}  // namespace core::threading

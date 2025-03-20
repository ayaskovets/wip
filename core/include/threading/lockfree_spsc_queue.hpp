#pragma once

#include <atomic>
#include <format>
#include <optional>
#include <span>
#include <type_traits>
#include <vector>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"

namespace core::threading {

template <typename ItemType, std::size_t Capacity = std::dynamic_extent>
  requires(std::is_nothrow_destructible_v<ItemType> &&
           std::is_nothrow_move_constructible_v<ItemType> &&
           sizeof(ItemType) <= utils::kCacheLineSize)
class lockfree_spsc_queue final {
  struct alignas(utils::kCacheLineSize) AlignedItemType {
    ItemType value;
  };

 public:
  constexpr lockfree_spsc_queue()
    requires(Capacity != std::dynamic_extent)
      : ring_buffer_(static_cast<AlignedItemType*>(
            std::malloc(sizeof(AlignedItemType) * (Capacity + 1)))) {
    if (!ring_buffer_) {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr explicit lockfree_spsc_queue(std::size_t capacity)
    requires(Capacity == std::dynamic_extent)
      : ring_buffer_(static_cast<AlignedItemType*>(
            std::malloc(sizeof(AlignedItemType) * (capacity + 1)))),
        capacity_(capacity) {
    if (!ring_buffer_) {
      throw std::runtime_error("failed to allocate memory");
    }
  }

  constexpr ~lockfree_spsc_queue() {
    while (try_pop().has_value()) {
    }
    std::free(ring_buffer_);
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 public:
  constexpr bool try_push(ItemType value) noexcept {
    const auto push_to = push_to_.load(std::memory_order::relaxed);
    const auto next_push_to = (push_to == *capacity_) ? 0 : push_to + 1;
    if (next_push_to == cached_pop_from_ &&
        (next_push_to ==
         (cached_pop_from_ = pop_from_.load(std::memory_order::acquire)))) {
      return false;
    }
    std::construct_at(&reinterpret_cast<ItemType&>(ring_buffer_[push_to]),
                      std::move(value));
    push_to_.store(next_push_to, std::memory_order::release);
    return true;
  }

  constexpr std::optional<ItemType> try_pop() noexcept {
    std::optional<ItemType> value(std::nullopt);
    const auto pop_from = pop_from_.load(std::memory_order::relaxed);
    if (pop_from == cached_push_to_ &&
        (pop_from ==
         (cached_push_to_ = push_to_.load(std::memory_order::acquire)))) {
      return value;
    }
    value.emplace(
        std::move(reinterpret_cast<ItemType&>(ring_buffer_[pop_from])));
    std::destroy_at(&ring_buffer_[pop_from]);
    const auto next_pop_from = (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return value;
  }

 private:
  AlignedItemType* ring_buffer_ = nullptr;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_pop_from_ = 0;
};

}  // namespace core::threading

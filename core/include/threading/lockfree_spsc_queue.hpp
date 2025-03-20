#pragma once

#include <atomic>
#include <cerrno>
#include <format>
#include <optional>
#include <span>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"

namespace core::threading {

template <typename T, std::size_t Capacity = std::dynamic_extent>
  requires(std::is_nothrow_destructible_v<T> &&
           std::is_nothrow_move_constructible_v<T>)
class lockfree_spsc_queue final : utils::non_copyable, utils::non_movable {
 private:
  struct alignas(utils::kCacheLineSize) AlignedT {
    T value;
  };

  static_assert(sizeof(AlignedT) == utils::kCacheLineSize);
  static_assert(alignof(AlignedT) == utils::kCacheLineSize);

 private:
  constinit static inline auto kAllocator = [](std::size_t size) {
    return static_cast<AlignedT*>(std::malloc(sizeof(AlignedT) * size));
  };

  constinit static inline auto kDeleter = [](AlignedT* item) {
    std::free(item);
  };

 public:
  constexpr lockfree_spsc_queue()
    requires(Capacity != std::dynamic_extent)
      : ring_buffer_(kAllocator(Capacity + 1)) {
    if (!ring_buffer_) {
      throw std::runtime_error(
          std::format("failed to allocate memory: {}", std::strerror(errno)));
    }
  }

  constexpr explicit lockfree_spsc_queue(std::size_t capacity)
    requires(Capacity == std::dynamic_extent)
      : ring_buffer_(kAllocator(capacity + 1)), capacity_(capacity) {
    if (!ring_buffer_) {
      throw std::runtime_error(
          std::format("failed to allocate memory :{}", std::strerror(errno)));
    }
  }

  constexpr ~lockfree_spsc_queue() noexcept {
    while (try_pop().has_value()) {
    }
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

    std::construct_at(&reinterpret_cast<T&>(ring_buffer_.get()[push_to]),
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

    value.emplace(
        std::move(reinterpret_cast<T&>(ring_buffer_.get()[pop_from])));

    std::destroy_at(&ring_buffer_.get()[pop_from]);
    const auto next_pop_from = (pop_from == *capacity_) ? 0 : pop_from + 1;
    pop_from_.store(next_pop_from, std::memory_order::release);
    return value;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  std::unique_ptr<AlignedT, decltype(kDeleter)> ring_buffer_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;

  alignas(utils::kCacheLineSize) std::atomic<std::size_t> push_to_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_push_to_ = 0;
  alignas(utils::kCacheLineSize) std::atomic<std::size_t> pop_from_ = 0;
  alignas(utils::kCacheLineSize) std::size_t cached_pop_from_ = 0;
};

}  // namespace core::threading

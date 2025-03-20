#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <optional>
#include <span>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"

namespace core::threading {

template <typename ItemType, std::size_t Capacity = std::dynamic_extent>
  requires(std::is_nothrow_destructible_v<ItemType> &&
           std::is_nothrow_move_constructible_v<ItemType>)
class locked_mpmc_queue final {
 public:
  constexpr locked_mpmc_queue() noexcept
    requires(Capacity != std::dynamic_extent)
  = default;

  constexpr explicit locked_mpmc_queue(std::size_t capacity) noexcept
    requires(Capacity == std::dynamic_extent)
      : capacity_(capacity) {}

  constexpr explicit locked_mpmc_queue() noexcept
    requires(Capacity == std::dynamic_extent)
      : capacity_(std::dynamic_extent) {}

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }
  std::size_t size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
  }

 public:
  void push(ItemType value) {
    std::unique_lock<std::mutex> lock(mutex_);
    push_available_cv_.wait(lock,
                            [this] { return queue_.size() < *capacity_; });
    queue_.push_back(std::move(value));
    pop_available_cv_.notify_one();
  }

  ItemType pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    pop_available_cv_.wait(lock, [this] { return !queue_.empty(); });
    ItemType front(
        std::move(queue_.front()));  // NOTE: correct iff noexcept(pop_front)
    queue_.pop_front();
    push_available_cv_.notify_one();
    return front;
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable pop_available_cv_;
  std::condition_variable push_available_cv_;
  std::deque<ItemType> queue_;
  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;
};

}  // namespace core::threading

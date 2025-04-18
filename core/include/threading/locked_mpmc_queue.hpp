#pragma once

#include <condition_variable>
#include <deque>
#include <mutex>
#include <type_traits>

#include "utils/conditionally_runtime.hpp"
#include "utils/constants.hpp"
#include "utils/mixins.hpp"

namespace core::threading {

template <typename T, std::size_t Capacity = utils::kRuntimeCapacity,
          typename Allocator = std::allocator<T>>
class locked_mpmc_queue final : utils::non_copyable, utils::non_movable {
 private:
  static_assert(std::is_nothrow_destructible_v<T>,
                "T is required to be nothrow move destructible to remove "
                "copies in push() and try_push()");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T is required to be nothrow move constructible to remove "
                "copies in pop()");
  static_assert(std::is_nothrow_move_assignable_v<T>,
                "T is required to be nothrow move assignable to remove "
                "copies in try_pop()");
  static_assert(std::is_same_v<T, typename Allocator::value_type>,
                "T and allocator value_type must be the same type");

 public:
  constexpr explicit locked_mpmc_queue(
      const Allocator& allocator = Allocator()) noexcept
    requires(Capacity != utils::kRuntimeCapacity && Capacity > 0)
      : queue_(allocator) {}

  constexpr explicit locked_mpmc_queue(std::size_t capacity,
                                       const Allocator& allocator = Allocator())
    requires(Capacity == utils::kRuntimeCapacity)
      : queue_(allocator), capacity_(capacity) {
    if (*capacity_ <= 0) {
      throw std::invalid_argument("capacity must be greater than zero");
    }
  }

 public:
  bool try_push(T value) {
    if (size_.load(std::memory_order::relaxed) >= *capacity_) {
      return false;
    }
    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock_t{});
    if (!lock.try_lock()) {
      return false;
    }
    if (queue_.size() >= *capacity_) {
      return false;
    }

    queue_.push_back(std::move(value));
    pop_available_cv_.notify_one();
    size_.fetch_add(1, std::memory_order::relaxed);
    return true;
  }

  void push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    push_available_cv_.wait(lock,
                            [this] { return queue_.size() < *capacity_; });

    queue_.push_back(std::move(value));
    size_.fetch_add(1, std::memory_order::relaxed);
    pop_available_cv_.notify_one();
  }

  bool try_pop(T& value) {
    if (size_.load(std::memory_order::relaxed) == 0) {
      return false;
    }
    std::unique_lock<std::mutex> lock(mutex_, std::defer_lock_t{});
    if (!lock.try_lock()) {
      return false;
    }
    if (queue_.empty()) {
      return false;
    }

    value = std::move(queue_.front());
    queue_.pop_front();
    size_.fetch_sub(1, std::memory_order::relaxed);
    push_available_cv_.notify_one();
    return true;
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    pop_available_cv_.wait(lock, [this] { return !queue_.empty(); });

    // NOTE: correct iff noexcept(pop_front)
    T front(std::move(queue_.front()));

    queue_.pop_front();
    size_.fetch_sub(1, std::memory_order::relaxed);
    push_available_cv_.notify_one();
    return front;
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }

 private:
  mutable std::mutex mutex_;
  std::condition_variable pop_available_cv_;
  std::condition_variable push_available_cv_;

  std::deque<T, Allocator> queue_;
  std::atomic<std::size_t> size_;

  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == utils::kRuntimeCapacity, Capacity> capacity_;
};

}  // namespace core::threading

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
                "copies in push()");
  static_assert(std::is_nothrow_move_constructible_v<T>,
                "T is required to be nothrow move destructible to remove "
                "copies in pop()");

 public:
  struct unbounded_queue_t final {};

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

  constexpr explicit locked_mpmc_queue(
      unbounded_queue_t, const Allocator& allocator = Allocator()) noexcept
    requires(Capacity == utils::kRuntimeCapacity)
      : queue_(allocator), capacity_(std::numeric_limits<std::size_t>::max()) {}

 public:
  void push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    push_available_cv_.wait(lock, [this] {
      return stop_requested_.test(std::memory_order::relaxed) ||
             queue_.size() < *capacity_;
    });

    if (stop_requested_.test(std::memory_order::release)) {
      throw std::runtime_error("push() cancelled");
    }

    queue_.push_back(std::move(value));
    pop_available_cv_.notify_one();
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    pop_available_cv_.wait(lock, [this] {
      return stop_requested_.test(std::memory_order::relaxed) ||
             !queue_.empty();
    });

    if (stop_requested_.test(std::memory_order::release)) {
      throw std::runtime_error("pop() cancelled");
    }

    // NOTE: correct iff noexcept(pop_front)
    T front(std::move(queue_.front()));

    queue_.pop_front();
    push_available_cv_.notify_one();
    return front;
  }

 public:
  constexpr void request_stop() noexcept {
    stop_requested_.test_and_set(std::memory_order::acquire);
    pop_available_cv_.notify_all();
    push_available_cv_.notify_all();
  }

 public:
  constexpr std::size_t capacity() const noexcept { return *capacity_; }
  std::size_t size() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return queue_.size();
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable pop_available_cv_;
  std::condition_variable push_available_cv_;
  std::atomic_flag stop_requested_ = false;

  std::deque<T> queue_;

  [[no_unique_address]] const utils::conditionally_runtime<
      std::size_t, Capacity == utils::kRuntimeCapacity, Capacity> capacity_;
};

}  // namespace core::threading

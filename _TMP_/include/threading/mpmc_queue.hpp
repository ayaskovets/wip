#pragma once

#include <deque>
#include <mutex>
#include <optional>
#include <span>

#include "utils/conditionally_runtime.hpp"

namespace _TMP_::threading {

template <typename T, std::size_t Capacity = std::dynamic_extent>
class mpmc_queue final {
 public:
  constexpr mpmc_queue()
    requires(Capacity != std::dynamic_extent)
  = default;
  constexpr mpmc_queue(std::size_t capacity) : capacity_(capacity) {}

 public:
  constexpr std::size_t capacity() const { return *capacity_; }
  std::size_t size() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
  }

 public:
  bool try_push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.size() == *capacity_) {
      return false;
    }
    queue_.push_back(std::move(value));
    pop_available_cv_.notify_one();
    return true;
  }

  void push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.size() == *capacity_) {
      push_available_cv_.wait(lock,
                              [this]() { return queue_.size() < *capacity_; });
    }
    queue_.push_back(std::move(value));
    pop_available_cv_.notify_one();
  }

  std::optional<T> try_pop() {
    std::optional<T> front = std::nullopt;
    {
      std::unique_lock<std::mutex> lock(mutex_);
      if (!queue_.empty()) {
        front.emplace(queue_.front());
        queue_.pop_front();
      }
      push_available_cv_.notify_one();
    }
    return front;
  }

  T pop() {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.empty()) {
      pop_available_cv_.wait(lock, [this]() { return !queue_.empty(); });
    }
    T front = queue_.front();
    queue_.pop_front();
    push_available_cv_.notify_one();
    return front;
  }

 private:
  mutable std::mutex mutex_;
  std::condition_variable pop_available_cv_;
  std::condition_variable push_available_cv_;
  std::deque<T> queue_;
  [[no_unique_address]] utils::conditionally_runtime<
      std::size_t, Capacity == std::dynamic_extent, Capacity> capacity_;
};

}  // namespace _TMP_::threading

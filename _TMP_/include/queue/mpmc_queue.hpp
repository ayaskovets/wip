#pragma once

#include <deque>
#include <mutex>
#include <semaphore>

namespace _TMP_::queue {

template <typename T>
class mpmc_queue final {
 public:
  constexpr mpmc_queue(std::size_t max_size) : max_size_(max_size) {}

 public:
  void push(T value) {
    std::unique_lock<std::mutex> lock(mutex_);
    if (queue_.size() == max_size_) {
      push_available_cv_.wait(lock,
                              [this]() { return queue_.size() < max_size_; });
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

  std::size_t size() {
    std::unique_lock<std::mutex> lock(mutex_);
    return queue_.size();
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
  std::mutex mutex_;
  std::condition_variable pop_available_cv_;
  std::condition_variable push_available_cv_;
  const std::size_t max_size_;
  std::deque<T> queue_;
};

}  // namespace _TMP_::queue

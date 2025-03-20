#include <benchmark/benchmark.h>

#include <queue>
#include <span>

#include "threading/mpmc_queue.hpp"

template <typename T>
class BM_locked_std_queue : public benchmark::Fixture {
 protected:
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable pop_available_;
};

BENCHMARK_TEMPLATE_DEFINE_F(BM_locked_std_queue, pod_type, int)
(benchmark::State& state) {
  if (state.thread_index() % 2) {
    for (const auto _ : state) {
      std::unique_lock<std::mutex> lock(mutex_);
      queue_.push(42);
      pop_available_.notify_one();
    }
  } else {
    for (const auto _ : state) {
      std::unique_lock<std::mutex> lock(mutex_);
      pop_available_.wait(lock, [this]() { return !queue_.empty(); });
      const int value = queue_.front();
      queue_.pop();
      benchmark::DoNotOptimize(&value);
    }
  }
}

BENCHMARK_TEMPLATE_DEFINE_F(BM_locked_std_queue, heap_allocated,
                            std::shared_ptr<int>)
(benchmark::State& state) {
  if (state.thread_index() % 2) {
    for (const auto _ : state) {
      std::unique_lock<std::mutex> lock(mutex_);
      queue_.push(std::make_shared<int>(42));
      pop_available_.notify_one();
    }
  } else {
    for (const auto _ : state) {
      std::unique_lock<std::mutex> lock(mutex_);
      pop_available_.wait(lock, [this]() { return !queue_.empty(); });
      const std::shared_ptr<int> value = queue_.front();
      queue_.pop();
      benchmark::DoNotOptimize(&value);
    }
  }
}

template <typename T>
class BM_mpmc_queue : public benchmark::Fixture {
 protected:
  _TMP_::threading::mpmc_queue<T, 100> queue_;
};

BENCHMARK_TEMPLATE_DEFINE_F(BM_mpmc_queue, pod_type, int)
(benchmark::State& state) {
  if (state.thread_index() % 2) {
    for (const auto _ : state) {
      queue_.push(42);
    }
  } else {
    for (const auto _ : state) {
      const int value = queue_.pop();
      benchmark::DoNotOptimize(&value);
    }
  }
}

BENCHMARK_TEMPLATE_DEFINE_F(BM_mpmc_queue, heap_allocated, std::shared_ptr<int>)
(benchmark::State& state) {
  if (state.thread_index() % 2) {
    for (const auto _ : state) {
      queue_.push(std::make_shared<int>(42));
    }
  } else {
    for (const auto _ : state) {
      const std::shared_ptr<int> value = queue_.pop();
      benchmark::DoNotOptimize(&value);
    }
  }
}

BENCHMARK_REGISTER_F(BM_locked_std_queue, pod_type)
    ->ThreadRange(2, 1 << 4)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_REGISTER_F(BM_mpmc_queue, pod_type)
    ->ThreadRange(2, 1 << 4)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_REGISTER_F(BM_locked_std_queue, heap_allocated)
    ->ThreadRange(2, 1 << 4)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

BENCHMARK_REGISTER_F(BM_mpmc_queue, heap_allocated)
    ->ThreadRange(2, 1 << 4)
    ->MeasureProcessCPUTime()
    ->UseRealTime();

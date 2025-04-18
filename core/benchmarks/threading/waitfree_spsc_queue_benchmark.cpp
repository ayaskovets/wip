#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/waitfree_spsc_queue.hpp"

namespace benchmarks::threading {

template <typename ValueConstructor>
void BM_threading_waitfree_spsc_queue_nonblocking_throughput_spsc(
    benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const ValueConstructor value{};

  using queue_value_t = ValueConstructor;
  struct alignas(core::utils::kCacheLineSize) allocator_value_t final {
    queue_value_t value;
  };

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::waitfree_spsc_queue<queue_value_t,
                                         core::utils::kRuntimeCapacity,
                                         std::allocator<allocator_value_t>>
        queue(capacity);
    std::latch latch(3);

    std::thread producer([&latch, &queue, items, value = value] {
      latch.arrive_and_wait();
      for (std::size_t i = 0; i < items;) {
        i += queue.try_push(value);
      }
    });

    std::thread consumer([&latch, &queue, items] {
      latch.arrive_and_wait();
      for (std::size_t i = 0; i < items;) {
        queue_value_t value;
        i += queue.try_pop(value);
      }
    });

    state.ResumeTiming();

    latch.arrive_and_wait();
    producer.join();
    consumer.join();
  }
}
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_nonblocking_throughput_spsc,
                   int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_nonblocking_throughput_spsc,
                   std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

template <typename ValueConstructor>
void BM_threading_waitfree_spsc_queue_blocking_throughput_spsc(
    benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const ValueConstructor value{};

  using queue_value_t = ValueConstructor;
  struct alignas(core::utils::kCacheLineSize) allocator_value_t final {
    queue_value_t value;
  };

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::waitfree_spsc_queue<queue_value_t,
                                         core::utils::kRuntimeCapacity,
                                         std::allocator<allocator_value_t>>
        queue(capacity);
    std::latch latch(3);

    std::thread producer([&latch, &queue, items, value = value] {
      latch.arrive_and_wait();
      for (std::size_t i = 0; i < items; ++i) {
        queue.push(value);
      }
    });

    std::thread consumer([&latch, &queue, items] {
      latch.arrive_and_wait();
      for (std::size_t i = 0; i < items; ++i) {
        queue.pop();
      }
    });

    state.ResumeTiming();

    latch.arrive_and_wait();
    producer.join();
    consumer.join();
  }
}
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_blocking_throughput_spsc,
                   int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_blocking_throughput_spsc,
                   std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace benchmarks::threading

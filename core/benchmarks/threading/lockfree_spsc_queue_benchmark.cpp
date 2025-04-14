#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/lockfree_spsc_queue.hpp"
#include "utils/aligned.hpp"
#include "utils/constants.hpp"

namespace benchmarks::threading {

template <typename ValueConstructor>
void BM_threading_lockfree_spsc_queue_spsc_throughput(benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);

  using value_type = decltype(ValueConstructor());
  const value_type value{};

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::lockfree_spsc_queue<
        value_type, core::utils::kRuntimeCapacity,
        std::allocator<
            core::utils::aligned<value_type, core::utils::kCacheLineSize>>>
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
        i += queue.try_pop().has_value();
      }
    });

    state.ResumeTiming();

    latch.arrive_and_wait();
    producer.join();
    consumer.join();
  }
}
BENCHMARK_TEMPLATE(BM_threading_lockfree_spsc_queue_spsc_throughput, int)
    ->Args({1023, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_lockfree_spsc_queue_spsc_throughput,
                   std::shared_ptr<int>)
    ->Args({1023, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace benchmarks::threading

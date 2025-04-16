#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/waitfree_spsc_queue.hpp"
#include "utils/constants.hpp"

namespace benchmarks::threading {

template <typename ValueConstructor>
void BM_threading_waitfree_spsc_queue_spsc_throughput(benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);

  using value_t = decltype(ValueConstructor());
  const value_t value{};

  struct alignas(core::utils::kCacheLineSize) allocator_value_t final {
    value_t value;
  };
  using allocator_t = std::allocator<allocator_value_t>;

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::waitfree_spsc_queue<value_t, core::utils::kRuntimeCapacity,
                                         allocator_t>
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
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_spsc_throughput, int)
    ->Args({1023, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_waitfree_spsc_queue_spsc_throughput,
                   std::shared_ptr<int>)
    ->Args({1023, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace benchmarks::threading

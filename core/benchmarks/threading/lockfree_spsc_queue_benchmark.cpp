#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/lockfree_spsc_queue.hpp"

template <typename ValueConstructor>
void BM_locked_spsc_queue_spsc_throughput(benchmark::State& state) {
  const auto capacity = state.range(0);
  const auto items = state.range(1);
  const auto value = ValueConstructor();

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::lockfree_spsc_queue<std::decay_t<decltype(value)>> queue(
        capacity);
    std::latch latch(1 + 1 + 1);

    std::thread producer([&latch, &queue, items, value = value] {
      latch.arrive_and_wait();
      for (std::int64_t i = 0; i < items;) {
        i += queue.try_push(value);
      }
    });

    std::thread consumer([&latch, &queue, items] {
      latch.arrive_and_wait();
      for (std::int64_t i = 0; i < items;) {
        i += queue.try_pop().has_value();
      }
    });

    state.ResumeTiming();

    latch.arrive_and_wait();
    producer.join();
    consumer.join();
  }
}
BENCHMARK_TEMPLATE(BM_locked_spsc_queue_spsc_throughput, int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_locked_spsc_queue_spsc_throughput, std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

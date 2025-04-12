#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/lockfree_mpmc_queue.hpp"

namespace benchmarks::threading {

template <typename ValueConstructor>
void BM_threading_lockfree_mpmc_queue_spsc_throughput(benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const auto value = ValueConstructor();

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::lockfree_mpmc_queue<std::decay_t<decltype(value)>> queue(
        capacity);
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
BENCHMARK_TEMPLATE(BM_threading_lockfree_mpmc_queue_spsc_throughput, int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_lockfree_mpmc_queue_spsc_throughput,
                   std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

template <typename ValueConstructor>
void BM_threading_lockfree_mpmc_queue_mpmc_throughput(benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const std::size_t producers = state.range(2);
  const std::size_t consumers = state.range(3);
  const auto value = ValueConstructor();

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::lockfree_mpmc_queue<std::decay_t<decltype(value)>> queue(
        capacity);
    std::latch latch(producers + consumers + static_cast<std::size_t>(1));

    std::vector<std::thread> threads;
    threads.reserve(producers + consumers);

    std::atomic<std::size_t> pushed = 0;
    for (std::size_t i = 0; i < producers; ++i) {
      threads.emplace_back(([&latch, &queue, items, &pushed, value = value] {
        latch.arrive_and_wait();
        while (pushed.fetch_add(1, std::memory_order::relaxed) < items) {
          while (!queue.try_push(value)) {
          }
        }
      }));
    }

    std::atomic<std::size_t> popped = 0;
    for (std::size_t i = 0; i < consumers; ++i) {
      threads.emplace_back([&latch, &queue, items, &popped] {
        latch.arrive_and_wait();
        while (popped.fetch_add(1, std::memory_order::relaxed) < items) {
          while (!queue.try_pop().has_value()) {
          }
        }
      });
    }

    state.ResumeTiming();

    latch.arrive_and_wait();
    for (auto& thread : threads) {
      thread.join();
    }
  }
}
BENCHMARK_TEMPLATE(BM_threading_lockfree_mpmc_queue_mpmc_throughput, int)
    ->Args({1024, 1048576, 2, 2})
    ->Args({1024, 1048576, 1, 4})
    ->Args({1024, 1048576, 4, 4})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_lockfree_mpmc_queue_mpmc_throughput,
                   std::shared_ptr<int>)
    ->Args({256, 1048576, 2, 2})
    ->Args({256, 1048576, 1, 4})
    ->Args({256, 1048576, 4, 1})
    ->Args({256, 1048576, 4, 4})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

}  // namespace benchmarks::threading

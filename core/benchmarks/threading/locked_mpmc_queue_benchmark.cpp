#include <benchmark/benchmark.h>

#include <latch>
#include <thread>

#include "threading/locked_mpmc_queue.hpp"

namespace benchmarks::threading {

template <typename ValueConstructor>
void BM_threading_locked_mpmc_queue_nonblocking_throughput_spsc(
    benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const ValueConstructor value{};

  using queue_value_t = ValueConstructor;
  using allocator_value_t = ValueConstructor;

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::locked_mpmc_queue<queue_value_t,
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
BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_nonblocking_throughput_spsc,
                   int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_nonblocking_throughput_spsc,
                   std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

template <typename ValueConstructor>
void BM_threading_locked_mpmc_queue_blocking_throughput_spsc(
    benchmark::State& state) {
  const std::size_t capacity = state.range(0);
  const std::size_t items = state.range(1);
  const ValueConstructor value{};

  using queue_value_t = ValueConstructor;
  using allocator_value_t = ValueConstructor;

  for (const auto _ : state) {
    state.PauseTiming();

    core::threading::locked_mpmc_queue<queue_value_t,
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
BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_blocking_throughput_spsc, int)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);
BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_blocking_throughput_spsc,
                   std::shared_ptr<int>)
    ->Args({1024, 1048576})
    ->MeasureProcessCPUTime()
    ->UseRealTime()
    ->Unit(benchmark::kMillisecond);

// template <typename ValueConstructor>
// void BM_threading_locked_mpmc_queue_mpmc_throughput(benchmark::State& state)
// {
//   const std::size_t capacity = state.range(0);
//   const std::size_t items = state.range(1);
//   const std::size_t producers = state.range(2);
//   const std::size_t consumers = state.range(3);

//   using queue_value_t = decltype(ValueConstructor());
//   const queue_value_t value{};

//   for (const auto _ : state) {
//     state.PauseTiming();

//     core::threading::locked_mpmc_queue<queue_value_t> queue(capacity);
//     std::latch latch(producers + consumers + 1);

//     std::vector<std::thread> threads;
//     threads.reserve(producers + consumers);

//     std::atomic<std::size_t> pushed = 0;
//     for (std::size_t i = 0; i < producers; ++i) {
//       threads.emplace_back(([&latch, &queue, items, &pushed, value = value] {
//         latch.arrive_and_wait();
//         while (pushed.fetch_add(1, std::memory_order::relaxed) < items) {
//           queue.push(value);
//         }
//       }));
//     }

//     std::atomic<std::size_t> popped = 0;
//     for (std::size_t i = 0; i < consumers; ++i) {
//       threads.emplace_back([&latch, &queue, items, &popped] {
//         latch.arrive_and_wait();
//         while (popped.fetch_add(1, std::memory_order::relaxed) < items) {
//           const auto value = queue.pop();
//           benchmark::DoNotOptimize(&value);
//         }
//       });
//     }

//     state.ResumeTiming();

//     latch.arrive_and_wait();
//     for (auto& thread : threads) {
//       thread.join();
//     }
//   }
// }
// BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_mpmc_throughput, int)
//     ->Args({1024, 1048576, 2, 2})
//     ->Args({1024, 1048576, 1, 4})
//     ->Args({1024, 1048576, 4, 1})
//     ->Args({1024, 1048576, 4, 4})
//     ->MeasureProcessCPUTime()
//     ->UseRealTime()
//     ->Unit(benchmark::kMillisecond);
// BENCHMARK_TEMPLATE(BM_threading_locked_mpmc_queue_mpmc_throughput,
//                    std::shared_ptr<int>)
//     ->Args({1024, 1048576, 2, 2})
//     ->Args({1024, 1048576, 1, 4})
//     ->Args({1024, 1048576, 4, 1})
//     ->Args({1024, 1048576, 4, 4})
//     ->MeasureProcessCPUTime()
//     ->UseRealTime()
//     ->Unit(benchmark::kMillisecond);

}  // namespace benchmarks::threading

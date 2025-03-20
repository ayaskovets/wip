#include <benchmark/benchmark.h>

#include "datetime/serialization.hpp"

namespace benchmarks::datetime {

namespace datetime = todo::datetime;

void BM_iso8601(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    datetime::to_iso8601(now);
  }
}

void BM_format(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    const auto __ = std::format("{}", now);
  }
}

}  // namespace benchmarks::datetime

BENCHMARK(benchmarks::datetime::BM_iso8601);
BENCHMARK(benchmarks::datetime::BM_format);

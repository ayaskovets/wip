#include <benchmark/benchmark.h>

#include "datetime/iso8601.hpp"

namespace benchmarks::datetime {

namespace datetime = todo::datetime;

void BM_as_iso8601(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(datetime::as_iso8601(now));
  }
}

void BM_serialize_format(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(std::format("{}", now));
  }
}

}  // namespace benchmarks::datetime

BENCHMARK(benchmarks::datetime::BM_as_iso8601);
BENCHMARK(benchmarks::datetime::BM_serialize_format);

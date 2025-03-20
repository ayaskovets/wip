#include <benchmark/benchmark.h>

#include "datetime/iso8601.hpp"

namespace benchmarks::datetime {

namespace datetime = todo::datetime;

void BM_to_iso8601(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    [[maybe_unused]] const auto __ = datetime::to_iso8601(now);
  }
}

void BM_serialize_format(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    [[maybe_unused]] const auto __ = std::format("{}", now);
  }
}

}  // namespace benchmarks::datetime

BENCHMARK(benchmarks::datetime::BM_to_iso8601);
BENCHMARK(benchmarks::datetime::BM_serialize_format);

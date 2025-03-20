#include <benchmark/benchmark.h>

#include "datetime/iso8601.hpp"

void BM_as_iso8601(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(_TMP_::datetime::as_iso8601(now));
  }
}

void BM_serialize_format(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(std::format("{}", now));
  }
}

BENCHMARK(BM_as_iso8601);
BENCHMARK(BM_serialize_format);

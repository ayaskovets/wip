#include <benchmark/benchmark.h>

#include "datetime/iso8601.hpp"

void BM_datetime_as_iso8601(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::datetime::as_iso8601(now));
  }
}
BENCHMARK(BM_datetime_as_iso8601);

void BM_datetime_serialize_chrono_with_std_format(benchmark::State& state) {
  const auto now = std::chrono::system_clock::now();
  for (const auto _ : state) {
    benchmark::DoNotOptimize(std::format("{}", now));
  }
}
BENCHMARK(BM_datetime_serialize_chrono_with_std_format);

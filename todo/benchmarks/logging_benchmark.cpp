#include <benchmark/benchmark.h>

#include "logging/logging.hpp"

namespace benchmarks::logging {

namespace logging = todo::logging;

void BM_default(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize((logging::info("{}\n", 42), _));
  }
}

void BM_transparent(benchmark::State& state) {
  const logging::logger logger(logging::level::kInfo,
                               logging::renderer::kNoop(),
                               logging::writer::kStderr());
  for (const auto _ : state) {
    benchmark::DoNotOptimize((logger(logging::level::kInfo, "{}\n", 42), _));
  }
}

void BM_fprintf(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(fprintf(stderr, "%d\n", 42));
  }
}

void BM_fputs(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(fputs("42\n", stderr));
  }
}

}  // namespace benchmarks::logging

BENCHMARK(benchmarks::logging::BM_default);
BENCHMARK(benchmarks::logging::BM_transparent);
BENCHMARK(benchmarks::logging::BM_fprintf);
BENCHMARK(benchmarks::logging::BM_fputs);

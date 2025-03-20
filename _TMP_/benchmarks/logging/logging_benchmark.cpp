#include <benchmark/benchmark.h>

#include "logging/logging.hpp"

void BM_default(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize((_TMP_::logging::info("{}\n", 42), 42));
  }
}

void BM_transparent(benchmark::State& state) {
  const _TMP_::logging::logger logger(_TMP_::logging::level::kInfo,
                                      _TMP_::logging::renderer::kNoop(),
                                      _TMP_::logging::writer::kStderr());
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        (logger(_TMP_::logging::level::kInfo, "{}\n", 42), 42));
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

BENCHMARK(BM_default);
BENCHMARK(BM_transparent);
BENCHMARK(BM_fprintf);
BENCHMARK(BM_fputs);

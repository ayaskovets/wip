#include <benchmark/benchmark.h>

#include "ip/dns.hpp"

void BM_resolve_localhost(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        _TMP_::ip::resolve("localhost", _TMP_::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_localhost)->Unit(benchmark::kMicrosecond);

void BM_resolve_google(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        _TMP_::ip::resolve("google.com", _TMP_::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_google)->Unit(benchmark::kMicrosecond);

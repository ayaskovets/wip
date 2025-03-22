#include <benchmark/benchmark.h>

#include "ip/dns.hpp"

namespace benchmarks::ip {

void BM_resolve_localhost(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::ip::resolve("localhost", core::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_localhost)->Unit(benchmark::kMicrosecond);

void BM_resolve_google(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::ip::resolve("google.com", core::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_google)->Unit(benchmark::kMicrosecond);

}  // namespace benchmarks::ip

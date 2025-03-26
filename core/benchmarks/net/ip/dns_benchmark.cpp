#include <benchmark/benchmark.h>

#include "net/ip/dns.hpp"

namespace benchmarks::net::ip {

void BM_resolve_localhost(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::net::ip::resolve("localhost", core::net::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_localhost)->Unit(benchmark::kMicrosecond);

void BM_resolve_google(benchmark::State& state) {
  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::net::ip::resolve("google.com", core::net::ip::protocol::kTcp));
  }
}
BENCHMARK(BM_resolve_google)->Unit(benchmark::kMicrosecond);

}  // namespace benchmarks::net::ip

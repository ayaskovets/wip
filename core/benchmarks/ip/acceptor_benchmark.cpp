#include <benchmark/benchmark.h>

#include "ip/acceptor.hpp"

void BM_acceptor_try_accept(benchmark::State& state) {
  const core::ip::acceptor acceptor(
      core::ip::address::kLocalhost(core::ip::version::kIpV4),
      core::ip::port(9996));

  for (const auto _ : state) {
    benchmark::DoNotOptimize(acceptor.try_accept());
  }
}
BENCHMARK(BM_acceptor_try_accept);

#include <benchmark/benchmark.h>

#include "ip/tcp/acceptor.hpp"

namespace benchmarks::ip::tcp {

void BM_acceptor_try_accept(benchmark::State& state) {
  const core::ip::tcp::acceptor acceptor(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIpV4),
          core::ip::port(9996)),
      1);

  for (const auto _ : state) {
    benchmark::DoNotOptimize(acceptor.try_accept());
  }
}
BENCHMARK(BM_acceptor_try_accept);

}  // namespace benchmarks::ip::tcp

#include <benchmark/benchmark.h>

#include "net/ip/tcp/acceptor.hpp"

namespace benchmarks::net::ip::tcp {

void BM_acceptor_try_accept(benchmark::State& state) {
  const core::net::ip::tcp::acceptor acceptor(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9996)),
      1);

  for (const auto _ : state) {
    benchmark::DoNotOptimize(acceptor.try_accept());
  }
}
BENCHMARK(BM_acceptor_try_accept);

}  // namespace benchmarks::net::ip::tcp

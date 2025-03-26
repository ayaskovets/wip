#include <benchmark/benchmark.h>

#include "ip/tcp/acceptor.hpp"
#include "ip/tcp/connection.hpp"

namespace benchmarks::ip::tcp {

void BM_connection_try_accept(benchmark::State& state) {
  const core::ip::tcp::acceptor acceptor(
      core::ip::endpoint(
          core::ip::address::kLocalhost(core::ip::version::kIPv4),
          core::ip::port(9996)),
      1);

  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::ip::tcp::connection::try_accept(acceptor));
  }
}
BENCHMARK(BM_connection_try_accept);

void BM_connection_try_connect(benchmark::State& state) {
  const core::ip::endpoint endpoint(
      core::ip::address::kLocalhost(core::ip::version::kIPv4),
      core::ip::port(9996));

  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::ip::tcp::connection::try_connect(endpoint));
  }
}
BENCHMARK(BM_connection_try_connect)->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::ip::tcp

#include <benchmark/benchmark.h>

#include "net/ip/tcp/acceptor.hpp"
#include "net/ip/tcp/connection.hpp"

namespace benchmarks::net::ip::tcp {

void BM_connection_try_accept(benchmark::State& state) {
  const core::net::ip::tcp::acceptor acceptor(
      core::net::ip::endpoint(
          core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
          core::net::ip::port(9996)),
      1);

  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::net::ip::tcp::connection::try_accept(acceptor));
  }
}
BENCHMARK(BM_connection_try_accept);

void BM_connection_try_connect(benchmark::State& state) {
  const core::net::ip::endpoint endpoint(
      core::net::ip::address::kLocalhost(core::net::ip::version::kIPv4),
      core::net::ip::port(9996));

  for (const auto _ : state) {
    benchmark::DoNotOptimize(
        core::net::ip::tcp::connection::try_connect(endpoint));
  }
}
BENCHMARK(BM_connection_try_connect)->Unit(benchmark::TimeUnit::kMicrosecond);

}  // namespace benchmarks::net::ip::tcp

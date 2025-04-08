#include <benchmark/benchmark.h>

#include "net/unix/sockaddr.hpp"

namespace benchmarks::net::unix {

void BM_net_unix_from_string(benchmark::State& state) {
  const std::string string("BM_net_unix_from_string_unix_sockaddr");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::unix::sockaddr(string));
  }
}
BENCHMARK(BM_net_unix_from_string);

void BM_net_unix_to_string(benchmark::State& state) {
  const core::net::unix::sockaddr sockaddr(
      "BM_net_unix_from_string_unix_sockaddr");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(sockaddr.to_string());
  }
}
BENCHMARK(BM_net_unix_to_string);

}  // namespace benchmarks::net::unix

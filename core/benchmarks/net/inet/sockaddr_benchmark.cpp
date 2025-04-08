#include <benchmark/benchmark.h>

#include "net/inet/sockaddr.hpp"

namespace benchmarks::net::inet {

void BM_net_inet_sockaddr_from_bytes(benchmark::State& state) {
  const std::uint32_t bytes(0x7f000001);
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::inet::sockaddr(
        core::net::inet::ip(bytes), core::net::inet::port(0)));
  }
}
BENCHMARK(BM_net_inet_sockaddr_from_bytes);

void BM_net_inet_sockaddr_from_string(benchmark::State& state) {
  const std::string string("127.0.0.1");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::inet::sockaddr(
        core::net::inet::ip(string), core::net::inet::port(0)));
  }
}
BENCHMARK(BM_net_inet_sockaddr_from_string);

void BM_net_inet_sockaddr_to_string(benchmark::State& state) {
  const core::net::inet::sockaddr sockaddr(core::net::inet::ip("127.0.0.1"),
                                           core::net::inet::port(0));
  for (const auto _ : state) {
    benchmark::DoNotOptimize(sockaddr.to_string());
  }
}
BENCHMARK(BM_net_inet_sockaddr_to_string);

}  // namespace benchmarks::net::inet

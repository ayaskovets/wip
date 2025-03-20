#include <benchmark/benchmark.h>

#include "ip/address.hpp"
#include "ip/dns.hpp"

namespace benchmarks::ip {

namespace ip = todo::ip;

void BM_ip4_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{127, 0, 0, 1};
  for (const auto _ : state) {
    const auto __ = ip::address(bytes);
  }
}

void BM_ip4_from_string(benchmark::State& state) {
  const std::string string("127.0.0.1");
  for (const auto _ : state) {
    const auto __ = ip::address(string);
  }
}

void BM_ip4_as_string(benchmark::State& state) {
  const auto address = ip::address("127.0.0.1");
  for (const auto _ : state) {
    address.as_string();
  }
}

void BM_ip6_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255};
  for (const auto _ : state) {
    const auto __ = ip::address(std::move(bytes));
  }
}

void BM_ip6_from_string(benchmark::State& state) {
  const std::string string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    const auto __ = ip::address(std::move(string));
  }
}

void BM_ip6_as_string(benchmark::State& state) {
  const auto address = ip::address("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    address.as_string();
  }
}

void BM_resolve_localhost(benchmark::State& state) {
  for (const auto _ : state) {
    ip::resolve("localhost", ip::protocol::kTcp);
  }
}

void BM_resolve_google(benchmark::State& state) {
  for (const auto _ : state) {
    ip::resolve("google.com", ip::protocol::kTcp);
  }
}

}  // namespace benchmarks::ip

BENCHMARK(benchmarks::ip::BM_ip4_from_bytes);
BENCHMARK(benchmarks::ip::BM_ip4_from_string);
BENCHMARK(benchmarks::ip::BM_ip4_as_string);
BENCHMARK(benchmarks::ip::BM_ip6_from_bytes);
BENCHMARK(benchmarks::ip::BM_ip6_from_string);
BENCHMARK(benchmarks::ip::BM_ip6_as_string);
BENCHMARK(benchmarks::ip::BM_resolve_localhost);
BENCHMARK(benchmarks::ip::BM_resolve_google);

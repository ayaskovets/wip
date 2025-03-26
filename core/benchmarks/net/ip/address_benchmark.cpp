#include <benchmark/benchmark.h>

#include "net/ip/address.hpp"

namespace benchmarks::ip {

void BM_ip4_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{127, 0, 0, 1};
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::ip::address(bytes));
  }
}
BENCHMARK(BM_ip4_from_bytes);

void BM_ip6_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255};
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::ip::address(bytes));
  }
}
BENCHMARK(BM_ip6_from_bytes);

void BM_ip4_from_string(benchmark::State& state) {
  const std::string string("127.0.0.1");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::ip::address(string));
  }
}
BENCHMARK(BM_ip4_from_string);

void BM_ip6_from_string(benchmark::State& state) {
  const std::string string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(core::net::ip::address(string));
  }
}
BENCHMARK(BM_ip6_from_string);

void BM_ip4_to_string(benchmark::State& state) {
  const core::net::ip::address address("127.0.0.1");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(address.to_string());
  }
}
BENCHMARK(BM_ip4_to_string);

void BM_ip6_to_string(benchmark::State& state) {
  const auto address =
      core::net::ip::address("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(address.to_string());
  }
}
BENCHMARK(BM_ip6_to_string);

}  // namespace benchmarks::ip

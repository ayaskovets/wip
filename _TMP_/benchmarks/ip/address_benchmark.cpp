#include <benchmark/benchmark.h>

#include "ip/address.hpp"

void BM_ip4_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{127, 0, 0, 1};
  for (const auto _ : state) {
    benchmark::DoNotOptimize(_TMP_::ip::address(bytes));
  }
}
BENCHMARK(BM_ip4_from_bytes);

void BM_ip6_from_bytes(benchmark::State& state) {
  const std::vector<std::uint8_t> bytes{255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255};
  for (const auto _ : state) {
    benchmark::DoNotOptimize(_TMP_::ip::address(std::move(bytes)));
  }
}
BENCHMARK(BM_ip6_from_bytes);

void BM_ip4_from_string(benchmark::State& state) {
  const std::string string("127.0.0.1");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(_TMP_::ip::address(string));
  }
}
BENCHMARK(BM_ip4_from_string);

void BM_ip6_from_string(benchmark::State& state) {
  const std::string string("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(_TMP_::ip::address(std::move(string)));
  }
}
BENCHMARK(BM_ip6_from_string);

void BM_ip4_as_string(benchmark::State& state) {
  const _TMP_::ip::address address("127.0.0.1");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(address.as_string());
  }
}
BENCHMARK(BM_ip4_as_string);

void BM_ip6_as_string(benchmark::State& state) {
  const auto address =
      _TMP_::ip::address("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
  for (const auto _ : state) {
    benchmark::DoNotOptimize(address.as_string());
  }
}
BENCHMARK(BM_ip6_as_string);

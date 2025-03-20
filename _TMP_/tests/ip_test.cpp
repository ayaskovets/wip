#include <gtest/gtest.h>

#include "ip/address.hpp"
#include "ip/dns.hpp"

namespace tests::logging {

class _TMP__ip_address
    : public ::testing::TestWithParam<std::tuple<
          std::vector<std::uint8_t>, std::string, _TMP_::ip::version>> {};

TEST_P(_TMP__ip_address, construction) {
  const auto [bytes, string, version] = GetParam();

  const auto address1 = _TMP_::ip::address(bytes);
  const auto address2 = _TMP_::ip::address(string);

  EXPECT_EQ(address1.as_bytes(), bytes);
  EXPECT_EQ(address1.as_string(), string);
  EXPECT_EQ(address1.version(), version);

  EXPECT_EQ(address1.as_bytes(), address2.as_bytes());
  EXPECT_EQ(address1.as_string(), address2.as_string());
  EXPECT_EQ(address1.version(), address2.version());
  EXPECT_EQ(address1, address2);
}

INSTANTIATE_TEST_SUITE_P(
    _TMP__ip, _TMP__ip_address,
    ::testing::Values(
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255},
                        "255.255.255.255", _TMP_::ip::version::kIpV4),
        std::make_tuple(std::vector<std::uint8_t>{255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255, 255, 255,
                                                  255, 255, 255, 255},
                        "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff",
                        _TMP_::ip::version::kIpV6)));

TEST(_TMP__ip, resolve_localhost) {
  const auto addresses =
      _TMP_::ip::resolve("localhost", _TMP_::ip::protocol::kTcp);

  EXPECT_EQ(addresses,
            (std::vector<_TMP_::ip::address>{_TMP_::ip::address("::1"),
                                             _TMP_::ip::address("127.0.0.1")}));
}

}  // namespace tests::logging

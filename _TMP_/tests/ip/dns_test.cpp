#include "ip/dns.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(_TMP__ip, resolve_localhost) {
  const auto addresses =
      _TMP_::ip::resolve("localhost", _TMP_::ip::protocol::kTcp);
  EXPECT_EQ(addresses,
            (std::vector<_TMP_::ip::address>{_TMP_::ip::address("::1"),
                                             _TMP_::ip::address("127.0.0.1")}));
}

}  // namespace tests::ip

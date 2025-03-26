#include "net/ip/endpoint.hpp"

#include <gtest/gtest.h>

namespace tests::ip {

TEST(ip_endpoint, size) {
  static_assert(sizeof(core::net::ip::endpoint) == 20);
  static_assert(alignof(core::net::ip::endpoint) == 2);
}

}  // namespace tests::ip

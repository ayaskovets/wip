#pragma once

#include "net/ip/socket.hpp"

namespace core::net::ip::udp {

class socket final : public net::ip::socket {
 public:
  explicit socket(net::ip::version version);

 public:
  using net::ip::socket::receive;
  using net::ip::socket::send;

  using net::ip::socket::receive_from;
  using net::ip::socket::send_to;
};

}  // namespace core::net::ip::udp

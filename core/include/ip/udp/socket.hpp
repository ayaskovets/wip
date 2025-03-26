#pragma once

#include "ip/socket.hpp"

namespace core::ip::udp {

class socket final : public ip::socket {
 public:
  explicit socket(ip::version version);

 public:
  using ip::socket::receive;
  using ip::socket::send;

  using ip::socket::receive_from;
  using ip::socket::send_to;
};

}  // namespace core::ip::udp

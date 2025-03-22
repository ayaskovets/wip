#pragma once

#include "ip/socket.hpp"

namespace core::ip::udp {

class socket final : public ip::socket {
 public:
  explicit socket(ip::version version);
};

}  // namespace core::ip::udp

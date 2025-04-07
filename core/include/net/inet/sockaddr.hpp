#pragma once

#include "net/inet/ip.hpp"
#include "net/inet/port.hpp"
#include "net/sockets/base_sockaddr.hpp"

namespace core::net::inet {

class sockaddr final : public net::sockets::base_sockaddr {
 public:
  sockaddr(net::inet::ip ip, net::inet::port port);

 public:
  net::inet::ip get_ip() const noexcept;
  net::inet::port get_port() const noexcept;
};

}  // namespace core::net::inet

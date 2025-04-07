#pragma once

#include "net/inet6/ip.hpp"
#include "net/inet6/port.hpp"
#include "net/sockets/base_sockaddr.hpp"

namespace core::net::inet6 {

class sockaddr final : public net::sockets::base_sockaddr {
 public:
  sockaddr(net::inet6::ip ip, net::inet6::port port);

 public:
  net::inet6::ip get_ip() const noexcept;
  net::inet6::port get_port() const noexcept;
};

}  // namespace core::net::inet6

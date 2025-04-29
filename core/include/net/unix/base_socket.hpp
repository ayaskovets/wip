#pragma once

#include "net/sockets/base_socket.hpp"
#include "net/unix/sockaddr.hpp"

namespace core::net::unix {

class base_socket : public net::sockets::base_socket {
 protected:
  base_socket(utils::uninitialized_t) noexcept;
  base_socket(core::net::sockets::type type);
  ~base_socket() noexcept;

 public:
  void unlink_close();

 public:
  net::sockets::base_socket::bind_status unlink_bind(
      const net::unix::sockaddr& sockaddr);
};

}  // namespace core::net::unix

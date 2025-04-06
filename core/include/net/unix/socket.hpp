#pragma once

#include "net/sockets/base_socket.hpp"
#include "net/unix/sockaddr.hpp"

namespace core::net::unix {

class socket : public net::sockets::base_socket {
 protected:
  static const socket& kUninitialized() noexcept;

 private:
  socket() noexcept;

 protected:
  socket(core::net::sockets::type type);
  ~socket();

 public:
  void unlink_close();

 public:
  net::sockets::base_socket::bind_status unlink_bind(
      const net::unix::sockaddr& sockaddr);
};

}  // namespace core::net::unix

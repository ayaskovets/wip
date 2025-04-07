#pragma once

#include "net/sockets/base_socket.hpp"

namespace core::net::inet6::tcp {

class socket final : public net::sockets::base_socket {
 public:
  static const socket& kUninitialized() noexcept;

 private:
  socket(bool) noexcept;

 public:
  socket();
};

}  // namespace core::net::inet6::tcp

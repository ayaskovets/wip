#pragma once

#include "net/unix/base_socket.hpp"

namespace core::net::unix::dgram {

class socket final : public net::unix::base_socket {
 public:
  socket(utils::uninitialized_t) noexcept;
  socket();
};

}  // namespace core::net::unix::dgram

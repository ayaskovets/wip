#pragma once

#include "net/unix/socket.hpp"

namespace core::net::unix::dgram {

class socket final : public net::unix::socket {
 private:
  socket(int) noexcept;

 public:
  static const socket& kUninitialized() noexcept;

 public:
  socket();
};

}  // namespace core::net::unix::dgram

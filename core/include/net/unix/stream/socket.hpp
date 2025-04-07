#pragma once

#include "net/unix/socket.hpp"

namespace core::net::unix::stream {

class socket final : public net::unix::socket {
 public:
  static const socket& kUninitialized() noexcept;

 private:
  socket(bool) noexcept;

 public:
  socket();
};

}  // namespace core::net::unix::stream

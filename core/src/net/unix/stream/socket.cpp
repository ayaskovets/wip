#include "net/unix/stream/socket.hpp"

namespace core::net::unix::stream {

const socket& socket::kUninitialized() noexcept {
  static socket socket(bool{});
  return socket;
}

socket::socket(bool) noexcept
    : net::unix::socket(net::unix::socket::kUninitialized()) {}

socket::socket() : net::unix::socket(net::sockets::type::kStream) {}

}  // namespace core::net::unix::stream

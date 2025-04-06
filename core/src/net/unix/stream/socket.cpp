#include "net/unix/stream/socket.hpp"

namespace core::net::unix::stream {

socket::socket(int) noexcept
    : net::unix::socket(net::unix::socket::kUninitialized()) {}

const socket& socket::kUninitialized() noexcept {
  static socket socket(int{});
  return socket;
}

socket::socket() : net::unix::socket(net::sockets::type::kStream) {}

}  // namespace core::net::unix::stream

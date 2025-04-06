#include "net/unix/dgram/socket.hpp"

namespace core::net::unix::dgram {

socket::socket(int) noexcept
    : net::unix::socket(net::unix::socket::kUninitialized()) {}

const socket& socket::kUninitialized() noexcept {
  static socket socket(int{});
  return socket;
}

socket::socket() : net::unix::socket(net::sockets::type::kDgram) {}

}  // namespace core::net::unix::dgram

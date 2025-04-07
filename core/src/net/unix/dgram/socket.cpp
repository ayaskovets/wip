#include "net/unix/dgram/socket.hpp"

namespace core::net::unix::dgram {

const socket& socket::kUninitialized() noexcept {
  static socket socket(bool{});
  return socket;
}

socket::socket(bool) noexcept
    : net::unix::socket(net::unix::socket::kUninitialized()) {}

socket::socket() : net::unix::socket(net::sockets::type::kDgram) {}

}  // namespace core::net::unix::dgram

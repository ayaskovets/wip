#include "net/inet/udp/socket.hpp"

namespace core::net::inet::udp {

const socket& socket::kUninitialized() noexcept {
  static socket socket(bool{});
  return socket;
}

socket::socket(bool) noexcept
    : net::sockets::base_socket(net::sockets::base_socket::kUninitialized()) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet,
                                net::sockets::type::kDgram,
                                net::sockets::protocol::kUdp) {}

}  // namespace core::net::inet::udp

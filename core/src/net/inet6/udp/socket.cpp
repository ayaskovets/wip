#include "net/inet6/udp/socket.hpp"

namespace core::net::inet6::udp {

const socket& socket::kUninitialized() noexcept {
  static socket socket(bool{});
  return socket;
}

socket::socket(bool) noexcept
    : net::sockets::base_socket(net::sockets::base_socket::kUninitialized()) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet6,
                                net::sockets::type::kDgram,
                                net::sockets::protocol::kUdp) {}

}  // namespace core::net::inet6::udp

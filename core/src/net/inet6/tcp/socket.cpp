#include "net/inet6/tcp/socket.hpp"

namespace core::net::inet6::tcp {

const socket& socket::kUninitialized() noexcept {
  static socket socket(bool{});
  return socket;
}

socket::socket(bool) noexcept
    : net::sockets::base_socket(net::sockets::base_socket::kUninitialized()) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet6,
                                net::sockets::type::kStream,
                                net::sockets::protocol::kTcp) {}

}  // namespace core::net::inet6::tcp

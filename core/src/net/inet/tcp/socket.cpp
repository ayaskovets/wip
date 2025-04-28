#include "net/inet/tcp/socket.hpp"

namespace core::net::inet::tcp {

socket::socket(utils::uninitialized_t) noexcept
    : net::sockets::base_socket(core::utils::uninitialized_t{}) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet,
                                net::sockets::type::kStream,
                                net::sockets::protocol::kTcp) {}

}  // namespace core::net::inet::tcp

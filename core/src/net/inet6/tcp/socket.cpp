#include "net/inet6/tcp/socket.hpp"

namespace core::net::inet6::tcp {

socket::socket(utils::uninitialized_t) noexcept
    : net::sockets::base_socket(core::utils::uninitialized_t{}) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet6,
                                net::sockets::type::kStream,
                                net::sockets::protocol::kTcp) {}

}  // namespace core::net::inet6::tcp

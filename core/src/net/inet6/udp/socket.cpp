#include "net/inet6/udp/socket.hpp"

namespace core::net::inet6::udp {

socket::socket(utils::uninitialized_t) noexcept
    : net::sockets::base_socket(core::utils::uninitialized_t{}) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet6,
                                net::sockets::type::kDgram,
                                net::sockets::protocol::kUdp) {}

}  // namespace core::net::inet6::udp

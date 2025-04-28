#include "net/inet/udp/socket.hpp"

namespace core::net::inet::udp {

socket::socket(utils::uninitialized_t) noexcept
    : net::sockets::base_socket(core::utils::uninitialized_t{}) {}

socket::socket()
    : net::sockets::base_socket(net::sockets::family::kInet,
                                net::sockets::type::kDgram,
                                net::sockets::protocol::kUdp) {}

}  // namespace core::net::inet::udp

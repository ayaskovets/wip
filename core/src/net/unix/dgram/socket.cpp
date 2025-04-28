#include "net/unix/dgram/socket.hpp"

namespace core::net::unix::dgram {

socket::socket(utils::uninitialized_t) noexcept
    : net::unix::base_socket(core::utils::uninitialized_t{}) {}

socket::socket() : net::unix::base_socket(net::sockets::type::kDgram) {}

}  // namespace core::net::unix::dgram

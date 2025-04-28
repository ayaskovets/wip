#include "net/unix/stream/socket.hpp"

namespace core::net::unix::stream {

socket::socket(utils::uninitialized_t) noexcept
    : net::unix::base_socket(core::utils::uninitialized_t{}) {}

socket::socket() : net::unix::base_socket(net::sockets::type::kStream) {}

}  // namespace core::net::unix::stream

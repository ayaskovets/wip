#include "net/ip/udp/socket.hpp"

namespace core::net::ip::udp {

socket::socket(net::ip::version version)
    : net::ip::socket(net::ip::protocol::kUdp, version) {}

}  // namespace core::net::ip::udp

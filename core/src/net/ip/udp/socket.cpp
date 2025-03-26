#include "net/ip/udp/socket.hpp"

namespace core::net::ip::udp {

socket::socket(ip::version version)
    : net::ip::socket(ip::protocol::kUdp, version) {}

}  // namespace core::net::ip::udp

#include "ip/udp/socket.hpp"

namespace core::ip::udp {

socket::socket(ip::version version) : ip::socket(ip::protocol::kUdp, version) {}

}  // namespace core::ip::udp

#include "ip/udp/socket.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

namespace core::ip::udp {

socket::socket(ip::version version) : ip::socket(ip::protocol::kUdp, version) {}

}  // namespace core::ip::udp

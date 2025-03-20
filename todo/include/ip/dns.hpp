#pragma once

#include <string_view>

#include "ip/address.hpp"
#include "ip/protocol.hpp"

namespace todo::ip {

std::vector<address> resolve(std::string_view hostname, protocol protocol);

}  // namespace todo::ip

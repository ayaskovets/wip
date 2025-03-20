#pragma once

#include <optional>
#include <string_view>

#include "ip/address.hpp"
#include "ip/port.hpp"
#include "ip/protocol.hpp"

namespace core::ip {

std::vector<std::pair<ip::address, ip::port>> resolve(
    std::string_view hostname,
    std::optional<ip::protocol> protocol = std::nullopt,
    std::optional<ip::version> version = std::nullopt);

}  // namespace core::ip

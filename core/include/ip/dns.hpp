#pragma once

#include <optional>
#include <string_view>

#include "ip/address.hpp"
#include "ip/protocol.hpp"

namespace core::ip {

std::vector<address> resolve(std::string_view hostname,
                             std::optional<protocol> protocol = std::nullopt,
                             std::optional<version> version = std::nullopt);

}  // namespace core::ip

#pragma once

#include <optional>
#include <string_view>

#include "ip/endpoint.hpp"
#include "ip/protocol.hpp"
#include "ip/version.hpp"

namespace core::ip {

std::vector<ip::endpoint> resolve(
    std::string_view hostname,
    std::optional<ip::protocol> protocol = std::nullopt,
    std::optional<ip::version> version = std::nullopt);

}  // namespace core::ip

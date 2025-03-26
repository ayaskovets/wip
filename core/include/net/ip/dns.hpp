#pragma once

#include <optional>
#include <string_view>

#include "net/ip/endpoint.hpp"
#include "net/ip/protocol.hpp"
#include "net/ip/version.hpp"

namespace core::net::ip {

std::vector<net::ip::endpoint> resolve(
    std::string_view hostname,
    std::optional<net::ip::protocol> protocol = std::nullopt,
    std::optional<net::ip::version> version = std::nullopt);

}  // namespace core::net::ip

#include "ip/dns.hpp"

#include <netdb.h>

#include <cerrno>
#include <format>
#include <span>

#include "utils/declarative.hpp"

namespace core::ip {

namespace {

constexpr std::size_t kGuessResultsSize = 5;

std::pair<ip::address, ip::port> to_address_port_pair(
    const ::addrinfo &addrinfo) {
  switch (addrinfo.ai_family) {
    case AF_INET: {
      const auto *sockaddr =
          reinterpret_cast<const ::sockaddr_in *>(addrinfo.ai_addr);

      const auto bytes = std::span<const std::uint8_t>(
          &reinterpret_cast<const std::uint8_t &>(sockaddr->sin_addr),
          sizeof(::in_addr));

      return std::pair<ip::address, ip::port>(
          std::piecewise_construct, std::forward_as_tuple(bytes),
          std::forward_as_tuple(sockaddr->sin_port,
                                ip::port::network_byte_order));
    }
    case AF_INET6: {
      const auto *sockaddr =
          reinterpret_cast<const ::sockaddr_in6 *>(addrinfo.ai_addr);

      const auto bytes = std::span<const std::uint8_t>(
          &reinterpret_cast<const std::uint8_t &>(sockaddr->sin6_addr),
          sizeof(::in6_addr));

      return std::pair<ip::address, ip::port>(
          std::piecewise_construct, std::forward_as_tuple(bytes),
          std::forward_as_tuple(sockaddr->sin6_port,
                                ip::port::network_byte_order));
    }
    default:
      throw std::runtime_error(
          std::format("unexpected address family {}", addrinfo.ai_family));
  }
}

}  // namespace

std::vector<std::pair<ip::address, ip::port>> resolve(
    std::string_view hostname, std::optional<ip::protocol> protocol,
    std::optional<ip::version> version) {
  ::addrinfo *results = nullptr;
  const utils::scope_exit _([results]() noexcept {
    if (results) {
      ::freeaddrinfo(results);
    }
  });

  ::addrinfo hints{};
  hints.ai_family = [version] {
    if (!version.has_value()) {
      return AF_UNSPEC;
    }
    switch (*version) {
      case ip::version::kIpV4:
        return AF_INET;
      case ip::version::kIpV6:
        return AF_INET6;
    }
  }();
  hints.ai_socktype = [protocol] {
    if (!protocol.has_value()) {
      return 0;
    }
    switch (*protocol) {
      case ip::protocol::kTcp:
        return SOCK_STREAM;
      case ip::protocol::kUdp:
        return SOCK_DGRAM;
    }
  }();

  switch (const auto error =
              ::getaddrinfo(hostname.data(), nullptr, &hints, &results)) {
    case 0:
      break;
    case EAI_SYSTEM:
      throw std::runtime_error(
          std::format("failed to resolve any address at {}: {}", hostname,
                      std::strerror(errno)));
    default:
      throw std::runtime_error(
          std::format("failed to resolve any address at {}: {}", hostname,
                      ::gai_strerror(error)));
  }

  std::vector<std::pair<ip::address, ip::port>> addresses;
  addresses.reserve(kGuessResultsSize);
  for (const ::addrinfo *ptr = results; ptr; ptr = ptr->ai_next) {
    addresses.emplace_back(to_address_port_pair(*ptr));
  }
  return addresses;
}

}  // namespace core::ip

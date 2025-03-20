#include "ip/dns.hpp"

#include <netdb.h>

#include <cerrno>
#include <format>

#include "utils/declarative.hpp"

namespace core::ip {

namespace {

std::vector<std::uint8_t> as_bytes(const addrinfo &addrinfo) {
  std::vector<std::uint8_t> bytes;
  switch (addrinfo.ai_family) {
    case AF_INET:
      bytes.resize(sizeof(in_addr));
      std::memcpy(
          bytes.data(),
          &(reinterpret_cast<const sockaddr_in *>(addrinfo.ai_addr))->sin_addr,
          sizeof(in_addr));
      return bytes;
    case AF_INET6:
      bytes.resize(sizeof(in6_addr));
      std::memcpy(bytes.data(),
                  &(reinterpret_cast<const sockaddr_in6 *>(addrinfo.ai_addr))
                       ->sin6_addr,
                  sizeof(in6_addr));
      return bytes;
    default:
      throw std::runtime_error(
          std::format("unexpected address family {}", addrinfo.ai_family));
  }
}

}  // namespace

std::vector<ip::address> resolve(std::string_view hostname,
                                 std::optional<ip::protocol> protocol,
                                 std::optional<ip::version> version) {
  addrinfo *results = nullptr;
  const utils::scope_exit _([results] {
    if (results) {
      freeaddrinfo(results);
    }
  });

  addrinfo hints{};
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
              getaddrinfo(hostname.data(), nullptr, &hints, &results)) {
    case 0:
      break;
    case EAI_SYSTEM:
      throw std::runtime_error(
          std::format("failed to resolve any address at {}: {}", hostname,
                      strerror(errno)));
    default:
      throw std::runtime_error(
          std::format("failed to resolve any address at {}: {}", hostname,
                      gai_strerror(error)));
  }

  std::vector<ip::address> addresses;
  for (const addrinfo *ptr = results; ptr; ptr = ptr->ai_next) {
    addresses.emplace_back(as_bytes(*ptr));
  }
  return addresses;
}

}  // namespace core::ip

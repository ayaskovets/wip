#include "ip/dns.hpp"

#include <netdb.h>
#include <unistd.h>

#include <cerrno>
#include <format>

#include "utils/declarative.hpp"

namespace todo::ip {

namespace {

constexpr auto kSocktype(protocol protocol) noexcept {
  switch (protocol) {
    case protocol::kTcp:
      return SOCK_STREAM;
    case protocol::kUdp:
      return SOCK_DGRAM;
  }
}

std::vector<std::uint8_t> as_bytes(const addrinfo &addrinfo) {
  std::vector<std::uint8_t> bytes;
  switch (addrinfo.ai_family) {
    case AF_INET:
      bytes.resize(sizeof(in_addr));
      std::memcpy(bytes.data(),
                  &((const sockaddr_in *)addrinfo.ai_addr)->sin_addr,
                  sizeof(in_addr));
      return bytes;
    case AF_INET6:
      bytes.resize(sizeof(in6_addr));
      std::memcpy(bytes.data(),
                  &((const sockaddr_in6 *)addrinfo.ai_addr)->sin6_addr,
                  sizeof(in6_addr));
      return bytes;
    default:
      throw std::runtime_error(
          std::format("unexpected address family {}", addrinfo.ai_family));
  }
}

}  // namespace

std::vector<address> resolve(std::string_view hostname, protocol protocol) {
  addrinfo *results = nullptr;
  SCOPE_EXIT([results]() {
    if (results) {
      freeaddrinfo(results);
    }
  });

  const addrinfo hints{
      .ai_family = AF_UNSPEC,
      .ai_socktype = kSocktype(protocol),
      .ai_flags = 0,
      .ai_protocol = 0,
  };

  switch (const auto error =
              getaddrinfo(hostname.data(), nullptr, &hints, &results)) {
    case 0:
      break;
    case EAI_SYSTEM:
      throw std::runtime_error(
          std::format("failed to resolve any {} address at {}: {}", protocol,
                      hostname, strerror(errno)));
    default:
      throw std::runtime_error(
          std::format("failed to resolve any {} address at {}: {}", protocol,
                      hostname, gai_strerror(error)));
  }

  std::vector<address> addresses;
  for (const addrinfo *ptr = results; ptr; ptr = ptr->ai_next) {
    const auto s = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (s != -1) {
      close(s);
      addresses.emplace_back(as_bytes(*ptr));
    }
  }
  return addresses;
}

}  // namespace todo::ip

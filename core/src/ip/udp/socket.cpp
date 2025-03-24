#include "ip/udp/socket.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>

namespace core::ip::udp {

namespace {

constexpr int kSyscallError = -1;

::sockaddr_storage to_sockaddr_storage(ip::endpoint endpoint) {
  ::sockaddr_storage storage;
  switch (endpoint.get_address().get_version()) {
    case ip::version::kIpV4: {
      ::sockaddr_in &sockaddr = reinterpret_cast<::sockaddr_in &>(storage);

      sockaddr.sin_family = AF_INET;
      sockaddr.sin_port =
          endpoint.get_port().get_bytes(ip::port::network_byte_order);
      sockaddr.sin_len = sizeof(::sockaddr_in);
      std::memcpy(&sockaddr.sin_addr, endpoint.get_address().get_bytes().data(),
                  endpoint.get_address().get_bytes().size());
      break;
    }
    case ip::version::kIpV6: {
      ::sockaddr_in6 &sockaddr = reinterpret_cast<::sockaddr_in6 &>(storage);

      sockaddr.sin6_family = AF_INET6;
      sockaddr.sin6_port =
          endpoint.get_port().get_bytes(ip::port::network_byte_order);
      sockaddr.sin6_len = sizeof(::sockaddr_in6);
      std::memcpy(&sockaddr.sin6_addr,
                  endpoint.get_address().get_bytes().data(),
                  endpoint.get_address().get_bytes().size());
      break;
    }
  }
  return storage;
}

ip::endpoint to_endpoint(const ::sockaddr_storage &storage) {
  switch (storage.ss_family) {
    case AF_INET: {
      const auto sockaddr = reinterpret_cast<const ::sockaddr_in *>(&storage);

      const auto bytes = std::span<const std::uint8_t>(
          &reinterpret_cast<const std::uint8_t &>(sockaddr->sin_addr),
          sizeof(::in_addr));

      return ip::endpoint(
          ip::address(bytes),
          ip::port(sockaddr->sin_port, ip::port::network_byte_order));
    }
    case AF_INET6: {
      const auto sockaddr = reinterpret_cast<const ::sockaddr_in6 *>(&storage);

      const auto bytes = std::span<const std::uint8_t>(
          &reinterpret_cast<const std::uint8_t &>(sockaddr->sin6_addr),
          sizeof(::in6_addr));

      return ip::endpoint(
          ip::address(bytes),
          ip::port(sockaddr->sin6_port, ip::port::network_byte_order));
    }
    default:
      throw std::runtime_error(
          std::format("unexpected address family {}", storage.ss_family));
  }
}

}  // namespace

socket::socket(ip::version version) : ip::socket(ip::protocol::kUdp, version) {}

std::size_t socket::send(std::span<const std::uint8_t> bytes) const {
  const ssize_t sent = ::send(fd_, bytes.data(), bytes.size(), 0);
  if (sent == kSyscallError) {
    switch (errno) {
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        return 0;
      }
      throw std::runtime_error(
          std::format("send failed: {}", std::strerror(errno)));
    }
  }
  return sent;
}

std::size_t socket::send(std::span<const std::uint8_t> bytes,
                         const ip::endpoint &endpoint) const {
  const ::sockaddr_storage storage(to_sockaddr_storage(endpoint));
  const ssize_t sent =
      ::sendto(fd_, bytes.data(), bytes.size(), 0,
               &reinterpret_cast<const sockaddr &>(storage), storage.ss_len);
  if (sent == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
    throw std::runtime_error(
        std::format("send failed: {}", std::strerror(errno)));
  }
  return sent;
}

std::size_t socket::receive(std::span<std::uint8_t> bytes) const {
  const ssize_t received = ::recv(fd_, bytes.data(), bytes.size(), 0);
  if (received == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
    throw std::runtime_error(
        std::format("recv failed: {}", std::strerror(errno)));
  }
  return received;
}

std::size_t socket::receive(std::span<std::uint8_t> bytes,
                            ip::endpoint &endpoint) const {
  ::sockaddr_storage storage;
  ::socklen_t socklen = sizeof(storage);
  const ssize_t received =
      ::recvfrom(fd_, bytes.data(), bytes.size(), 0,
                 &reinterpret_cast<sockaddr &>(storage), &socklen);
  if (received == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
    throw std::runtime_error(
        std::format("recv failed: {}", std::strerror(errno)));
  }
  endpoint = to_endpoint(storage);
  return received;
}

}  // namespace core::ip::udp

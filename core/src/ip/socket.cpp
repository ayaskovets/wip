#include "ip/socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>

#include <cerrno>
#include <format>

namespace core::ip {

namespace {

constexpr int kSyscallError = -1;

constexpr int get_domain(ip::version version) noexcept {
  switch (version) {
    case ip::version::kIPv4:
      return AF_INET;
    case ip::version::kIPv6:
      return AF_INET6;
  }
}

constexpr int get_type(ip::protocol protocol) noexcept {
  switch (protocol) {
    case ip::protocol::kTcp:
      return SOCK_STREAM;
    case ip::protocol::kUdp:
      return SOCK_DGRAM;
  }
}

::sockaddr_storage to_sockaddr_storage(ip::endpoint endpoint) {
  ::sockaddr_storage storage;
  switch (endpoint.get_address().get_version()) {
    case ip::version::kIPv4: {
      ::sockaddr_in &sockaddr = reinterpret_cast<::sockaddr_in &>(storage);

      sockaddr.sin_family = AF_INET;
      sockaddr.sin_port =
          endpoint.get_port().get_bytes(ip::port::network_byte_order);
      sockaddr.sin_len = sizeof(::sockaddr_in);
      std::memcpy(&sockaddr.sin_addr, endpoint.get_address().get_bytes().data(),
                  endpoint.get_address().get_bytes().size());
      break;
    }
    case ip::version::kIPv6: {
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

socket::socket(io::fd fd) : io::fd(std::move(fd)) {}

socket::socket(ip::protocol protocol, ip::version version)
    : io::fd([protocol, version] {
        const int fd = ::socket(get_domain(version), get_type(protocol), 0);
        if (fd == kSyscallError) [[unlikely]] {
          throw std::runtime_error(
              std::format("failed to claim socket file descriptor: {}",
                          std::strerror(errno)));
        }
        return fd;
      }()) {}

void socket::set_flag(flag flag, bool value) {
  const int optval = static_cast<int>(value);
  switch (flag) {
    case flag::kNonblocking: {
      const int flags = ::fcntl(fd_, F_GETFL);
      if (flags == kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to get F_GETFL on socket: {}", std::strerror(errno)));
      }
      if (::fcntl(fd_, F_SETFL,
                  value ? flags | O_NONBLOCK : flags & ~O_NONBLOCK) ==
          kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to set O_NONBLOCK on socket: {}", std::strerror(errno)));
      }
      break;
    }
    case flag::kReuseaddr:
      if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval,
                       sizeof(optval)) == kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to set SO_REUSEADDR on socket: {}", std::strerror(errno)));
      }
      break;
    case flag::kReuseport:
      if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval,
                       sizeof(optval)) == kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to set SO_REUSEPORT on socket: {}", std::strerror(errno)));
      }
      break;
    case flag::kKeepalive:
      if (::setsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval,
                       sizeof(optval)) == kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to set SO_KEEPALIVE on socket: {}", std::strerror(errno)));
      }
      break;
  }
}

bool socket::get_flag(flag flag) const {
  int optval;
  ::socklen_t optlen = sizeof(optval);
  switch (flag) {
    case flag::kNonblocking: {
      const int flags = ::fcntl(fd_, F_GETFL);
      if (flags == kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to get O_NONBLOCK on socket: {}", std::strerror(errno)));
      }
      optval = (flags & O_NONBLOCK);
      break;
    }
    case flag::kReuseaddr:
      if (::getsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &optval, &optlen) ==
          kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to get SO_REUSEADDR on socket: {}", std::strerror(errno)));
      }
      break;
    case flag::kReuseport:
      if (::getsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &optval, &optlen) ==
          kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to get SO_REUSEPORT on socket: {}", std::strerror(errno)));
      }
      break;
    case flag::kKeepalive:
      if (::getsockopt(fd_, SOL_SOCKET, SO_KEEPALIVE, &optval, &optlen) ==
          kSyscallError) [[unlikely]] {
        throw std::runtime_error(std::format(
            "failed to get SO_KEEPALIVE on socket: {}", std::strerror(errno)));
      }
      break;
  }
  if (optlen != sizeof(optval)) [[unlikely]] {
    throw std::runtime_error("unexpected value of socket flag");
  }
  return static_cast<bool>(optval);
}

void socket::bind(const ip::endpoint &endpoint) {
  const ::sockaddr_storage storage(to_sockaddr_storage(endpoint));
  if (::bind(fd_, &reinterpret_cast<const sockaddr &>(storage),
             storage.ss_len) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(
        std::format("failed to bind socket: {}", std::strerror(errno)));
  }
}

socket::connection_status socket::connect(const ip::endpoint &endpoint) {
  const ::sockaddr_storage storage(to_sockaddr_storage(endpoint));
  if (::connect(fd_, &reinterpret_cast<const ::sockaddr &>(storage),
                storage.ss_len) == kSyscallError) {
    switch (errno) {
      case EISCONN:
        return connection_status::kSuccess;
      case EINPROGRESS:
        return connection_status::kPending;
      case ECONNREFUSED:
      case EALREADY:
        return connection_status::kFailure;
      [[unlikely]] default:
        throw std::runtime_error(
            std::format("failed to connect socket: {}", std::strerror(errno)));
    }
  }
  return connection_status::kSuccess;
}

ip::endpoint socket::get_bind_endpoint() const {
  ::sockaddr_storage storage;
  ::socklen_t socklen = sizeof(storage);
  if (::getsockname(fd_, &reinterpret_cast<sockaddr &>(storage), &socklen) ==
      kSyscallError) [[unlikely]] {
    throw std::runtime_error(
        std::format("failed to get bind address: {}", std::strerror(errno)));
  }
  return to_endpoint(storage);
}

ip::endpoint socket::get_connect_endpoint() const {
  ::sockaddr_storage storage;
  ::socklen_t socklen = sizeof(storage);
  if (::getpeername(fd_, &reinterpret_cast<sockaddr &>(storage), &socklen) ==
      kSyscallError) [[unlikely]] {
    throw std::runtime_error(
        std::format("failed to get peer address: {}", std::strerror(errno)));
  }
  return to_endpoint(storage);
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

std::size_t socket::send(std::span<const std::uint8_t> bytes) const {
  const ssize_t sent = ::send(fd_, bytes.data(), bytes.size(), 0);
  if (sent == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return 0;
    }
    throw std::runtime_error(
        std::format("send failed: {}", std::strerror(errno)));
  }
  return sent;
}

std::size_t socket::receive_from(std::span<std::uint8_t> bytes,
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
        std::format("recvfrom failed: {}", std::strerror(errno)));
  }
  endpoint = to_endpoint(storage);
  return received;
}

std::size_t socket::send_to(std::span<const std::uint8_t> bytes,
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
        std::format("sendto failed: {}", std::strerror(errno)));
  }
  return sent;
}

}  // namespace core::ip

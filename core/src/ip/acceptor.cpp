#include "ip/acceptor.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>

#include <span>

namespace core::ip {

namespace {

constexpr int kInvalidFd = -1;
constexpr int kBacklog = 256;
constexpr int kTrue = 1;

::sockaddr_storage to_sockaddr_storage(ip::address address, ip::port port) {
  ::sockaddr_storage storage;

  switch (address.get_version()) {
    case ip::version::kIpV4: {
      ::sockaddr_in sockaddr;
      std::memset(&sockaddr, 0, sizeof(sockaddr));

      sockaddr.sin_family = AF_INET;
      sockaddr.sin_port = htons(port.get_bytes());
      sockaddr.sin_len = sizeof(::sockaddr_in);
      std::memcpy(&sockaddr.sin_addr, address.get_bytes().data(),
                  address.get_bytes().size());

      std::memcpy(&storage, &sockaddr, sizeof(sockaddr));
      break;
    }
    case ip::version::kIpV6: {
      ::sockaddr_in6 sockaddr;
      std::memset(&sockaddr, 0, sizeof(sockaddr));

      sockaddr.sin6_family = AF_INET6;
      sockaddr.sin6_port = htons(port.get_bytes());
      sockaddr.sin6_len = sizeof(::sockaddr_in6);
      std::memcpy(&sockaddr.sin6_addr, address.get_bytes().data(),
                  address.get_bytes().size());

      std::memcpy(&storage, &sockaddr, sizeof(sockaddr));
      break;
    }
  }

  return storage;
}

}  // namespace

acceptor::acceptor(ip::address address, ip::port port)
    : fd_(kInvalidFd), address_(std::move(address)), port_(std::move(port)) {
  const int domain = [version = address_.get_version()] {
    switch (version) {
      case ip::version::kIpV4:
        return AF_INET;
      case ip::version::kIpV6:
        return AF_INET6;
    }
  }();
  const ::sockaddr_storage storage(to_sockaddr_storage(address_, port_));

  std::string initialization_error = [&] {
    if ((fd_ = ::socket(domain, SOCK_STREAM, 0)) < 0) {
      return std::format("failed to create socket: {}", std::strerror(errno));
    } else if (::bind(fd_, &reinterpret_cast<const sockaddr&>(storage),
                      storage.ss_len) < 0) {
      return std::format("failed to bind socket: {}", std::strerror(errno));
    } else if (::fcntl(fd_, F_SETFL, O_NONBLOCK) < 0) {
      return std::format("failed to set O_NONBLOCK on socket: {}",
                         std::strerror(errno));
    } else if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &kTrue,
                            sizeof(kTrue)) < 0) {
      return std::format("failed to set SO_REUSEADDR on socket: {}",
                         std::strerror(errno));
    } else if (::setsockopt(fd_, SOL_SOCKET, SO_REUSEPORT, &kTrue,
                            sizeof(kTrue)) < 0) {
      return std::format("failed to set SO_REUSEPORT on socket: {}",
                         std::strerror(errno));
    } else if (::listen(fd_, kBacklog) < 0) {
      return std::format("failed to set nonblocking mode on socket: {}",
                         std::strerror(errno));
    } else {
      return std::string();
    }
  }();

  if (!initialization_error.empty()) {
    if (::close(fd_)) {
      throw std::runtime_error(
          std::format("{}; and then failed to close socket: {}",
                      std::move(initialization_error), std::strerror(errno)));
    }
    throw std::runtime_error(std::move(initialization_error));
  }
}

acceptor::acceptor(acceptor&& that) noexcept
    : fd_(std::exchange(that.fd_, kInvalidFd)),
      address_(std::move(that.address_)),
      port_(std::move(that.port_)) {}

acceptor& acceptor::operator=(acceptor&& that) noexcept {
  new (this) acceptor(std::move(that));
  return *this;
}

acceptor::~acceptor() noexcept(false) {
  // NOTE: purpousely throwing an exception here as there is no other way to
  // indicate close() failure with RAII
  if (fd_ != kInvalidFd && ::close(fd_) < 0) {
    throw std::runtime_error(
        std::format("failed to close socket: {}", std::strerror(errno)));
  }
}

std::optional<ip::connection> acceptor::try_accept() const {
  std::optional<ip::connection> connection(std::nullopt);

  ::sockaddr_storage storage;
  ::socklen_t socklen = sizeof(storage);
  if (::accept(fd_, &reinterpret_cast<::sockaddr&>(storage), &socklen) < 0) {
    switch (errno) {
      case EAGAIN:
        return connection;
      default:
        throw std::runtime_error(
            std::format("accept failed: {}", std::strerror(errno)));
    }
  }

  switch (storage.ss_family) {
    case AF_INET: {
      const auto& sockaddr = reinterpret_cast<const ::sockaddr_in&>(storage);
      connection.emplace(ip::address(reinterpret_cast<const std::uint8_t(&)[4]>(
                             sockaddr.sin_addr)),
                         ip::port(ntohs(sockaddr.sin_port)));
      break;
    }
    case AF_INET6: {
      const auto& sockaddr = reinterpret_cast<const ::sockaddr_in6&>(storage);
      connection.emplace(ip::address(reinterpret_cast<const std::uint8_t(&)[4]>(
                             sockaddr.sin6_addr)),
                         ip::port(ntohs(sockaddr.sin6_port)));
      break;
    }
    default:
      throw std::runtime_error(
          std::format("unexpected address family: {}", storage.ss_family));
  }

  return connection;
}

ip::address acceptor::get_address() const noexcept { return address_; }

ip::port acceptor::get_port() const noexcept { return port_; }

}  // namespace core::ip

#include "ip/tcp/socket.hpp"

#include <sys/socket.h>

#include <cassert>

namespace core::ip::tcp {

namespace {

constexpr int kSyscallError = -1;

}  // namespace

socket::socket(ip::version version) : ip::socket(ip::protocol::kTcp, version) {}

void socket::listen(std::size_t backlog) {
  if (::listen(fd_, backlog) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(std::format(
        "failed to set nonblocking mode on socket: {}", std::strerror(errno)));
  }
}

std::optional<socket> socket::accept() const {
  const int fd = ::accept(fd_, nullptr, nullptr);
  if (fd == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
      return std::nullopt;
    }
    throw std::runtime_error(
        std::format("accept failed: {}", std::strerror(errno)));
  }
  return socket(fd);
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

}  // namespace core::ip::tcp

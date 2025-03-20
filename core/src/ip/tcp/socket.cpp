#include "ip/tcp/socket.hpp"

#include <sys/socket.h>

namespace core::ip::tcp {

namespace {

constexpr int kSyscallError = -1;

}  // namespace

socket::socket(ip::version version) : ip::socket(ip::protocol::kTcp, version) {}

std::size_t socket::send(std::span<const std::uint8_t>) const {
  // TODO: implement
  return {};
}

std::size_t socket::receive(std::span<std::uint8_t>) const {
  // TODO: implement
  return {};
}

void socket::listen(std::size_t backlog) {
  if (::listen(fd_, backlog) == kSyscallError) {
    throw std::runtime_error(std::format(
        "failed to set nonblocking mode on socket: {}", std::strerror(errno)));
  }
}

std::optional<socket> socket::accept() const {
  const int fd = ::accept(fd_, nullptr, nullptr);
  if (fd == kSyscallError) {
    switch (errno) {
      case EAGAIN:
        return std::nullopt;
      default:
        throw std::runtime_error(
            std::format("accept failed: {}", std::strerror(errno)));
    }
  }
  return socket(fd);
}

}  // namespace core::ip::tcp

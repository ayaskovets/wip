#include "net/ip/tcp/socket.hpp"

#include <sys/socket.h>

namespace core::net::ip::tcp {

namespace {

constexpr int kSyscallError = -1;

}  // namespace

socket::socket(net::ip::version version)
    : net::ip::socket(net::ip::protocol::kTcp, version) {}

void socket::listen(std::size_t backlog) {
  if (::listen(fd_, backlog) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(
        std::format("listen failed: {}", std::strerror(errno)));
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

}  // namespace core::net::ip::tcp

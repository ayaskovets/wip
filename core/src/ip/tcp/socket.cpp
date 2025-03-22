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

std::optional<socket> socket::try_accept() const {
  assert(get_flag(ip::socket::flag::kNonblocking));

  const int fd = ::accept(fd_, nullptr, nullptr);
  if (fd == kSyscallError) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) [[likely]] {
      return std::nullopt;
    }
    throw std::runtime_error(
        std::format("accept failed: {}", std::strerror(errno)));
  }
  return socket(fd);
}

socket socket::accept() const {
  const int fd = ::accept(fd_, nullptr, nullptr);
  if (fd == kSyscallError) [[unlikely]] {
    assert(errno != EAGAIN && errno != EWOULDBLOCK);
    throw std::runtime_error(
        std::format("accept failed: {}", std::strerror(errno)));
  }
  return socket(fd);
}

}  // namespace core::ip::tcp

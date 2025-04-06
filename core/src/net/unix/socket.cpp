#include "net/unix/socket.hpp"

#include <unistd.h>

namespace core::net::unix {

namespace {

constexpr int kSyscallError = -1;

}  // namespace

const socket& socket::kUninitialized() noexcept {
  static const socket socket;
  return socket;
}

socket::socket() noexcept
    : net::sockets::base_socket(net::sockets::base_socket::kUninitialized()) {}

socket::socket(net::sockets::type type)
    : net::sockets::base_socket(net::sockets::family::kUnix, type,
                                net::sockets::protocol::kUnspecified) {}

socket::~socket() {
  if (*this == kUninitialized()) {
    return;
  }

  net::unix::sockaddr sockaddr(net::unix::sockaddr::kEmpty());
  try {
    get_bind_sockaddr(sockaddr);
    static_cast<void>(::unlink(sockaddr.get_path().data()));
  } catch (...) {
  }
}

void socket::unlink_close() {
  if (*this == kUninitialized()) {
    return;
  }

  net::unix::sockaddr sockaddr(net::unix::sockaddr::kEmpty());
  get_bind_sockaddr(sockaddr);

  if (::unlink(sockaddr.get_path().data()) == kSyscallError) {
    if (errno != ENOENT) {
      throw std::runtime_error(
          std::format("unlink failed: {}", std::strerror(errno)));
    }
  }
  net::sockets::base_socket::close();
}

net::sockets::base_socket::bind_status socket::unlink_bind(
    const net::unix::sockaddr& sockaddr) {
  if (::unlink(sockaddr.get_path().data()) == kSyscallError) {
    if (errno != ENOENT) {
      throw std::runtime_error(
          std::format("unlink failed: {}", std::strerror(errno)));
    }
  }
  return net::sockets::base_socket::bind(sockaddr);
}

}  // namespace core::net::unix

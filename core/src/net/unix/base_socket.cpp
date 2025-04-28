#include "net/unix/base_socket.hpp"

#include <unistd.h>

namespace core::net::unix {

namespace {

constexpr int kSyscallError = -1;

}  // namespace

base_socket::base_socket(utils::uninitialized_t) noexcept
    : net::sockets::base_socket(core::utils::uninitialized_t{}) {}

base_socket::base_socket(net::sockets::type type)
    : net::sockets::base_socket(net::sockets::family::kUnix, type,
                                net::sockets::protocol::kUnspecified) {}

base_socket::~base_socket() {
  static const base_socket kUninitialized(utils::uninitialized_t{});
  if (*this == kUninitialized) {
    return;
  }

  net::unix::sockaddr sockaddr(net::unix::sockaddr::kEmpty());
  try {
    get_bind_sockaddr(sockaddr);
    static_cast<void>(::unlink(sockaddr.get_path().data()));
  } catch (...) {
  }
}

void base_socket::unlink_close() {
  static const base_socket kUninitialized(utils::uninitialized_t{});
  if (*this == kUninitialized) {
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

net::sockets::base_socket::bind_status base_socket::unlink_bind(
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

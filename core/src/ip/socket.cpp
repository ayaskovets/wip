#include "ip/socket.hpp"

#include <sys/socket.h>
#include <unistd.h>

namespace core::ip {

namespace {

constexpr int kSyscallError = -1;
constexpr int kInvalidFd = -1;

}  // namespace

socket::socket(ip::version version, ip::protocol protocol) noexcept(false) {
  const int domain = [version] {
    switch (version) {
      case ip::version::kIpV4:
        return AF_INET;
      case ip::version::kIpV6:
        return AF_INET6;
    }
  }();

  const int type = [protocol] {
    switch (protocol) {
      case ip::protocol::kTcp:
        return SOCK_STREAM;
      case ip::protocol::kUdp:
        return SOCK_DGRAM;
    }
  }();

  if ((fd_ = ::socket(domain, type, 0)) == kSyscallError) {
    throw std::runtime_error("failed to claim socket file descriptor");
  }
}

socket::socket(const socket& that) noexcept(false) {
  if ((fd_ = ::dup(that.fd_)) == kSyscallError) {
    throw std::runtime_error("failed to clone socket file descriptor");
  }
}

socket& socket::operator=(const socket& that) noexcept(false) {
  if (::dup2(that.fd_, fd_) == kSyscallError) {
    throw std::runtime_error("failed to clone socket file descriptor");
  }
  return *this;
}

socket::socket(socket&& that) noexcept
    : fd_(std::exchange(that.fd_, kInvalidFd)) {}

socket& socket::operator=(socket&& that) noexcept(false) {
  if (fd_ != kInvalidFd &&
      ::close(std::exchange(fd_, kInvalidFd)) == kSyscallError) {
    throw std::runtime_error(
        std::format("failed to close socket: {}", std::strerror(errno)));
  }

  fd_ = std::exchange(that.fd_, kInvalidFd);
  return *this;
}

socket::~socket() noexcept {
  if (fd_ != kInvalidFd) {
    [[maybe_unused]] const auto _ = ::close(std::exchange(fd_, kInvalidFd));
  }
}

void socket::close() noexcept(false) {
  if (fd_ != kInvalidFd && ::close(std::exchange(fd_, kInvalidFd)) == -1) {
    throw std::runtime_error(
        std::format("failed to close socket: {}", std::strerror(errno)));
  }
}

}  // namespace core::ip

#include "io/fd.hpp"

#include <unistd.h>

#include <cerrno>
#include <format>

namespace core::io {

namespace {

constexpr int kSyscallError = -1;
constexpr int kInvalidFd = -1;
constexpr int kAnyValidFd = 0;

}  // namespace

const fd& fd::kUninitialized() noexcept {
  static const fd fd([] {
    // NOTE: kAnyValidFd is required to bypass the constructor check of the file
    // descriptor value validity
    class fd fd(kAnyValidFd);
    fd.fd_ = kInvalidFd;
    return fd;
  }());
  return fd;
}

const fd& fd::kStdin() noexcept {
  static const fd fd(STDIN_FILENO);
  return fd;
}

const fd& fd::kStdout() noexcept {
  static const fd fd(STDOUT_FILENO);
  return fd;
}

const fd& fd::kStderr() noexcept {
  static const fd fd(STDERR_FILENO);
  return fd;
}

fd::fd(int fd) : fd_(fd) {
  if (fd_ < 0) [[unlikely]] {
    throw std::invalid_argument(std::format("invalid file descriptor: {}", fd));
  }
}

fd::fd(const fd& that) {
  if (that.fd_ == kInvalidFd) [[unlikely]] {
    fd_ = kInvalidFd;
  } else if ((fd_ = ::dup(that.fd_)) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(std::format("failed to clone file descriptor: {} ",
                                         std::strerror(errno)));
  }
}

fd& fd::operator=(const fd& that) {
  if (that.fd_ == kInvalidFd) [[unlikely]] {
    fd_ = kInvalidFd;
  } else if (fd_ != that.fd_ && ::dup2(that.fd_, fd_) == kSyscallError)
      [[unlikely]] {
    throw std::runtime_error(std::format("failed to clone file descriptor: {} ",
                                         std::strerror(errno)));
  }
  return *this;
}

fd::fd(fd&& that) noexcept : fd_(std::exchange(that.fd_, kInvalidFd)) {}

fd& fd::operator=(fd&& that) {
  if (fd_ != that.fd_ && fd_ != kInvalidFd &&
      ::close(std::exchange(fd_, kInvalidFd)) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(std::format("failed to close file descriptor: {}",
                                         std::strerror(errno)));
  }
  fd_ = std::exchange(that.fd_, kInvalidFd);
  return *this;
}

fd::~fd() noexcept {
  if (fd_ != kInvalidFd) [[unlikely]] {
    [[maybe_unused]] const auto _ = ::close(std::exchange(fd_, kInvalidFd));
  }
}

bool fd::operator==(const fd& that) const noexcept { return fd_ == that.fd_; }

bool fd::operator!=(const fd& that) const noexcept { return !operator==(that); }

void fd::close() {
  if (fd_ == kInvalidFd) [[unlikely]] {
    throw std::runtime_error(
        std::format("close() called on already closed file descriptor: {}",
                    std::strerror(errno)));
  }
  if (::close(std::exchange(fd_, kInvalidFd)) == kSyscallError) [[unlikely]] {
    throw std::runtime_error(std::format("failed to close file descriptor: {}",
                                         std::strerror(errno)));
  }
}

int fd::get_fd() const {
  if (fd_ == kInvalidFd) [[unlikely]] {
    throw std::runtime_error("get_fd() called on a closed file descriptor");
  }
  return fd_;
}

}  // namespace core::io

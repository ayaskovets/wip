#include "ip/connection.hpp"

#include <unistd.h>

namespace core::ip {

namespace {

constexpr int kInvalidFd = -1;

}  // namespace

connection::connection(ip::address address, ip::port port)
    : fd_(kInvalidFd), address_(std::move(address)), port_(std::move(port)) {}

connection::~connection() noexcept(false) {
  // NOTE: purpousely throwing an exception here as there is no other way to
  // indicate close() failure with RAII
  if (fd_ != kInvalidFd && ::close(fd_) < 0) {
    throw std::runtime_error(
        std::format("failed to close socket: {}", std::strerror(errno)));
  }
}

connection::connection(connection&& that) noexcept
    : fd_(std::exchange(that.fd_, kInvalidFd)),
      address_(std::move(that.address_)),
      port_(std::move(that.port_)) {}

connection& connection::operator=(connection&& that) {
  if (fd_ != kInvalidFd && ::close(this->fd_) < 0) {
    throw std::runtime_error(
        std::format("failed to close socket: {}", std::strerror(errno)));
  }

  this->fd_ = std::exchange(that.fd_, kInvalidFd);
  this->address_ = std::move(that.address_);
  this->port_ = std::move(that.port_);
  return *this;
}

ip::address connection::get_address() const noexcept { return address_; }

ip::port connection::get_port() const noexcept { return port_; }

}  // namespace core::ip

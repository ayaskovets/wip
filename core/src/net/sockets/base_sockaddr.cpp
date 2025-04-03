#include "net/sockets/base_sockaddr.hpp"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>

#include <cassert>
#include <cerrno>

namespace core::net::sockets {

class base_sockaddr::storage {};

base_sockaddr::base_sockaddr(net::sockets::family family) {
  switch (family) {
    case net::sockets::family::kInet:
      storage_.reset(reinterpret_cast<storage*>(new ::sockaddr_in{
          .sin_family = AF_INET,
      }));
      break;
    case net::sockets::family::kInet6:
      storage_.reset(reinterpret_cast<storage*>(new ::sockaddr_in6{
          .sin6_family = AF_INET6,
      }));
      break;
    case net::sockets::family::kUnix:
      storage_.reset(reinterpret_cast<storage*>(new ::sockaddr_un{
          .sun_family = AF_UNIX,
      }));
      break;
  }
}

base_sockaddr::storage* base_sockaddr::get_storage() noexcept {
  return storage_.get();
}

bool base_sockaddr::operator==(const base_sockaddr& that) const noexcept {
  return storage_ == that.storage_ ||
         !std::memcmp(storage_.get(), that.storage_.get(), get_length());
}

bool base_sockaddr::operator!=(const base_sockaddr& that) const noexcept {
  return !operator==(that);
}

std::size_t base_sockaddr::get_length() const {
  switch (reinterpret_cast<const ::sockaddr*>(storage_.get())->sa_family) {
    case AF_INET:
      return sizeof(::sockaddr_in);
    case AF_INET6:
      return sizeof(::sockaddr_in6);
    case AF_UNIX:
      return sizeof(::sockaddr_un);
    [[unlikely]] default:
      throw std::runtime_error("unimplemented");
  }
}

net::sockets::family base_sockaddr::get_family() const {
  switch (reinterpret_cast<const ::sockaddr*>(storage_.get())->sa_family) {
    case AF_INET:
      return net::sockets::family::kInet;
    case AF_INET6:
      return net::sockets::family::kInet6;
    case AF_UNIX:
      return net::sockets::family::kUnix;
    [[unlikely]] default:
      throw std::runtime_error("unimplemented");
  }
}

const base_sockaddr::storage* base_sockaddr::get_storage() const noexcept {
  return storage_.get();
}

std::string base_sockaddr::to_string() const {
  std::string string;
  switch (reinterpret_cast<const ::sockaddr*>(storage_.get())->sa_family) {
    case AF_INET:
      string.resize(INET_ADDRSTRLEN);
      if (!::inet_ntop(AF_INET, get_storage(), string.data(), string.size()))
          [[unlikely]] {
        throw std::runtime_error(
            std::format("inet_ntop failed: {}", std::strerror(errno)));
      }
      string.resize(string.find('\0'));
      break;
    case AF_INET6:
      string.resize(INET6_ADDRSTRLEN);
      if (!::inet_ntop(AF_INET6, get_storage(), string.data(), string.size()))
          [[unlikely]] {
        throw std::runtime_error(
            std::format("inet_ntop failed: {}", std::strerror(errno)));
      }
      string.resize(string.find('\0'));
      break;
    case AF_UNIX:
      string = reinterpret_cast<const ::sockaddr_un*>(storage_.get())->sun_path;
      break;
    [[unlikely]] default:
      throw std::runtime_error("uninitialized family");
  }
  return string;
}

}  // namespace core::net::sockets
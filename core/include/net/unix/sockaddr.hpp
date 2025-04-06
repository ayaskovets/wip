#pragma once

#include "net/sockets/base_sockaddr.hpp"

namespace core::net::unix {

class sockaddr final : public net::sockets::base_sockaddr {
 public:
  static const sockaddr& kInvalid() noexcept;
  static const sockaddr& kEmpty() noexcept;

 public:
  explicit sockaddr(std::string_view path);

 public:
  std::string_view get_path() const noexcept;
};

}  // namespace core::net::unix

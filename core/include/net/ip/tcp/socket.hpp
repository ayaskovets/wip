#pragma once

#include "net/ip/socket.hpp"

namespace core::net::ip::tcp {

class socket final : public net::ip::socket {
 private:
  using net::ip::socket::socket;

 public:
  explicit socket(ip::version version);

 public:
  void listen(std::size_t backlog);
  std::optional<socket> accept() const;

 public:
  using net::ip::socket::receive;
  using net::ip::socket::send;
};

}  // namespace core::net::ip::tcp

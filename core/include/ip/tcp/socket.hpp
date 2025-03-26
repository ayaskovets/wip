#pragma once

#include "ip/socket.hpp"

namespace core::ip::tcp {

class socket final : public ip::socket {
 private:
  using ip::socket::socket;

 public:
  explicit socket(ip::version version);

 public:
  void listen(std::size_t backlog);
  std::optional<socket> accept() const;

 public:
  using ip::socket::receive;
  using ip::socket::send;
};

}  // namespace core::ip::tcp

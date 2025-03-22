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
  std::optional<socket> try_accept() const;
  socket accept() const;
};

}  // namespace core::ip::tcp

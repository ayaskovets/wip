#pragma once

#include <optional>

#include "ip/endpoint.hpp"
#include "ip/tcp/socket.hpp"

namespace core::ip::tcp {

class acceptor final {
 public:
  acceptor(ip::endpoint endpoint, std::size_t backlog);

 public:
  std::optional<ip::tcp::socket> try_accept() const;

 public:
  const ip::endpoint& get_endpoint() const;

 private:
  ip::tcp::socket socket_;
  const ip::endpoint endpoint_;
};

}  // namespace core::ip::tcp

#pragma once

#include <optional>

#include "net/ip/endpoint.hpp"
#include "net/ip/tcp/socket.hpp"

namespace core::net::ip::tcp {

class acceptor final {
 public:
  acceptor(ip::endpoint endpoint, std::size_t backlog);

 public:
  std::optional<ip::tcp::socket> try_accept() const;
  // net::ip::tcp::socket accept() const;

 public:
  const net::ip::endpoint& get_endpoint() const;

 private:
  mutable net::ip::tcp::socket socket_;
  mutable bool is_blocking_;
  net::ip::endpoint endpoint_;
};

}  // namespace core::net::ip::tcp

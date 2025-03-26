#pragma once

#include "net/ip/tcp/acceptor.hpp"
#include "net/ip/tcp/socket.hpp"

namespace core::net::ip::tcp {

class connection final {
 private:
  explicit connection(ip::tcp::socket socket) noexcept;

 public:
  static std::optional<connection> try_accept(
      const net::ip::tcp::acceptor& acceptor);
  // static connection accept(const net::ip::tcp::acceptor& acceptor);
  static std::optional<connection> try_connect(
      const net::ip::endpoint& endpoint);
  // NOTE: attemts a new blocking connection
  static std::optional<connection> connect(const net::ip::endpoint& endpoint);

 public:
  std::size_t send(std::span<const std::uint8_t> bytes) const;
  std::size_t receive(std::span<std::uint8_t> bytes) const;

 public:
  net::ip::endpoint get_endpoint() const;

 private:
  net::ip::tcp::socket socket_;
};

}  // namespace core::net::ip::tcp

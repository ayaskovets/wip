#pragma once

#include "ip/tcp/acceptor.hpp"
#include "ip/tcp/socket.hpp"

namespace core::ip::tcp {

class connection final {
 private:
  explicit connection(ip::tcp::socket socket) noexcept;

 public:
  static std::optional<connection> try_accept(
      const ip::tcp::acceptor& acceptor);
  static connection accept(const ip::tcp::acceptor& acceptor);
  static std::optional<connection> try_connect(const ip::endpoint& endpoint);
  static std::optional<connection> connect(const ip::endpoint& endpoint);

 public:
  std::size_t send(std::span<const std::uint8_t> bytes) const;
  std::size_t receive(std::span<std::uint8_t> bytes) const;

 public:
  ip::endpoint get_endpoint() const;

 private:
  ip::tcp::socket socket_;
};

}  // namespace core::ip::tcp

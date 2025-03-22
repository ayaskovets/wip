#include "ip/tcp/connection.hpp"

namespace core::ip::tcp {

connection::connection(ip::tcp::socket socket) noexcept
    : socket_(std::move(socket)) {}

std::optional<connection> connection::try_accept(
    const ip::tcp::acceptor& acceptor) {
  if (std::optional<ip::tcp::socket> socket = acceptor.try_accept();
      socket.has_value()) {
    return connection(std::move(*socket));
  }
  return std::nullopt;
}

connection connection::accept(const ip::tcp::acceptor& acceptor) {
  return connection(acceptor.accept());
}

std::optional<connection> connection::try_connect(
    const ip::endpoint& endpoint) {
  ip::tcp::socket socket(endpoint.get_address().get_version());
  socket.set_flag(ip::socket::flag::kNonblocking, true);
  socket.set_flag(ip::socket::flag::kKeepalive, true);
  if (socket.connect(endpoint) == ip::socket::connection_status::kFailure) {
    return std::nullopt;
  }
  return connection(std::move(socket));
}

std::optional<connection> connection::connect(const ip::endpoint& endpoint) {
  ip::tcp::socket socket(endpoint.get_address().get_version());
  socket.set_flag(ip::socket::flag::kKeepalive, true);
  if (socket.connect(endpoint) != ip::socket::connection_status::kSuccess) {
    return std::nullopt;
  }
  return connection(std::move(socket));
}

std::size_t connection::send(std::span<const std::uint8_t>) const {
  // TODO: implement send
  return {};
}

std::size_t connection::receive(std::span<std::uint8_t>) const {
  // TODO: implement recv
  return {};
}

ip::endpoint connection::get_endpoint() const {
  return socket_.get_connect_endpoint();
}

}  // namespace core::ip::tcp

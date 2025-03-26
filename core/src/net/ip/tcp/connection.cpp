#include "net/ip/tcp/connection.hpp"

namespace core::net::ip::tcp {

connection::connection(net::ip::tcp::socket socket) noexcept
    : socket_(std::move(socket)) {}

std::optional<connection> connection::try_accept(
    const net::ip::tcp::acceptor& acceptor) {
  if (std::optional<net::ip::tcp::socket> socket = acceptor.try_accept();
      socket.has_value()) {
    return connection(std::move(*socket));
  }
  return std::nullopt;
}

// connection connection::accept(const net::ip::tcp::acceptor& acceptor) {
//   return connection(acceptor.accept());
// }

std::optional<connection> connection::try_connect(
    const net::ip::endpoint& endpoint) {
  net::ip::tcp::socket socket(endpoint.get_address().get_version());
  socket.set_flag(net::ip::socket::flag::kNonblocking, true);
  socket.set_flag(net::ip::socket::flag::kKeepalive, true);
  if (socket.connect(endpoint) ==
      net::ip::socket::connection_status::kFailure) {
    return std::nullopt;
  }
  return connection(std::move(socket));
}

std::optional<connection> connection::connect(
    const net::ip::endpoint& endpoint) {
  net::ip::tcp::socket socket(endpoint.get_address().get_version());
  socket.set_flag(net::ip::socket::flag::kKeepalive, true);
  if (socket.connect(endpoint) !=
      net::ip::socket::connection_status::kSuccess) {
    return std::nullopt;
  }
  return connection(std::move(socket));
}

std::size_t connection::send(std::span<const std::uint8_t> bytes) const {
  return socket_.send(bytes);
}

std::size_t connection::receive(std::span<std::uint8_t> bytes) const {
  return socket_.receive(bytes);
}

ip::endpoint connection::get_endpoint() const {
  return socket_.get_connect_endpoint();
}

}  // namespace core::net::ip::tcp

#include "ip/tcp/acceptor.hpp"

namespace core::ip::tcp {

acceptor::acceptor(ip::endpoint endpoint, std::size_t backlog)
    : socket_([&endpoint, backlog] {
        ip::tcp::socket socket(endpoint.get_address().get_version());
        socket.set_flag(ip::socket::flag::kNonblocking, true);
        socket.set_flag(ip::socket::flag::kReuseaddr, true);
        socket.set_flag(ip::socket::flag::kReuseport, true);
        socket.set_flag(ip::socket::flag::kKeepalive, true);
        socket.bind(endpoint);
        socket.listen(backlog);
        return socket;
      }()),
      endpoint_(std::move(endpoint)),
      is_blocking_(false) {}

std::optional<ip::tcp::socket> acceptor::try_accept() const {
  if (is_blocking_) [[unlikely]] {
    socket_.set_flag(ip::socket::flag::kNonblocking, true);
  }
  return socket_.try_accept();
}

ip::tcp::socket acceptor::accept() const {
  if (!is_blocking_) [[unlikely]] {
    socket_.set_flag(ip::socket::flag::kNonblocking, false);
  }
  return socket_.accept();
}

const ip::endpoint& acceptor::get_endpoint() const { return endpoint_; }

}  // namespace core::ip::tcp

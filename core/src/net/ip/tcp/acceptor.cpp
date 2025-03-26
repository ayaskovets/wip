#include "net/ip/tcp/acceptor.hpp"

namespace core::net::ip::tcp {

acceptor::acceptor(net::ip::endpoint endpoint, std::size_t backlog)
    : socket_([&endpoint, backlog] {
        net::ip::tcp::socket socket(endpoint.get_address().get_version());
        socket.set_flag(net::ip::socket::flag::kNonblocking, true);
        socket.set_flag(net::ip::socket::flag::kReuseaddr, true);
        socket.set_flag(net::ip::socket::flag::kReuseport, true);
        socket.set_flag(net::ip::socket::flag::kKeepalive, true);
        socket.bind(endpoint);
        socket.listen(backlog);
        return socket;
      }()),
      is_blocking_(false),
      endpoint_(std::move(endpoint)) {}

std::optional<net::ip::tcp::socket> acceptor::try_accept() const {
  if (is_blocking_) [[unlikely]] {
    socket_.set_flag(net::ip::socket::flag::kNonblocking, true);
    is_blocking_ = true;
  }
  return socket_.accept();
}

// net::ip::tcp::socket acceptor::accept() const {
//   if (!is_blocking_) [[unlikely]] {
//     socket_.set_flag(net::ip::socket::flag::kNonblocking, false);
//     is_blocking_ = false;
//   }
//   return socket_.accept();
// }

const net::ip::endpoint& acceptor::get_endpoint() const { return endpoint_; }

}  // namespace core::net::ip::tcp

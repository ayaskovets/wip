#include "ip/tcp/acceptor.hpp"

namespace core::ip::tcp {

acceptor::acceptor(ip::endpoint endpoint, std::size_t backlog)
    : socket_(endpoint.get_address().get_version()),
      endpoint_(std::move(endpoint)) {
  socket_.set_flag(ip::socket::flag::kNonblocking, true);
  socket_.set_flag(ip::socket::flag::kReuseaddr, true);
  socket_.set_flag(ip::socket::flag::kReuseport, true);
  socket_.set_flag(ip::socket::flag::kKeepalive, true);
  socket_.bind(endpoint_);
  socket_.listen(backlog);
}

std::optional<ip::tcp::socket> acceptor::try_accept() const {
  return socket_.accept();
}

const ip::endpoint& acceptor::get_endpoint() const { return endpoint_; }

}  // namespace core::ip::tcp

#pragma once

#include <optional>

#include "ip/address.hpp"
#include "ip/connection.hpp"
#include "ip/port.hpp"
#include "utils/mixins.hpp"

namespace core::ip {

class acceptor final : utils::non_copyable {
 public:
  acceptor(ip::address address, ip::port port);
  ~acceptor() noexcept(false);

 public:
  acceptor(acceptor&& that) noexcept;
  acceptor& operator=(acceptor&& that) noexcept;

 public:
  std::optional<ip::connection> try_accept() const;

 public:
  ip::address get_address() const noexcept;
  ip::port get_port() const noexcept;

 private:
  int fd_;
  ip::address address_;
  ip::port port_;
};

}  // namespace core::ip

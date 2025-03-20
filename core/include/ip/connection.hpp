#pragma once

#include "ip/address.hpp"
#include "ip/port.hpp"
#include "utils/mixins.hpp"

namespace core::ip {

class connection final : utils::non_copyable {
 public:
  connection(ip::address address, ip::port port);
  ~connection() noexcept(false);

 public:
  connection(connection&& that) noexcept;
  connection& operator=(connection&& that) noexcept;

 public:
  ip::address get_address() const noexcept;
  ip::port get_port() const noexcept;

 private:
  int fd_;
  ip::address address_;
  ip::port port_;
};

}  // namespace core::ip

#pragma once

#include "ip/socket.hpp"

namespace core::ip::tcp {

class socket final : public ip::socket {
 private:
  using ip::socket::socket;

 public:
  explicit socket(ip::version version);

 public:
  void listen(std::size_t backlog);
  std::optional<socket> accept() const;

 public:
  std::size_t send(std::span<const std::uint8_t> bytes) const;
  std::size_t receive(std::span<std::uint8_t> bytes) const;
};

}  // namespace core::ip::tcp

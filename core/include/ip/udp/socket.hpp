#pragma once

#include "ip/socket.hpp"

namespace core::ip::udp {

class socket final : public ip::socket {
 public:
  explicit socket(ip::version version);

 public:
  std::size_t send(std::span<const std::uint8_t> bytes) const;
  std::size_t receive(std::span<std::uint8_t> bytes) const;

 public:
  std::size_t send_to(std::span<const std::uint8_t> bytes,
                      const ip::endpoint& endpoint) const;
  std::size_t receive_from(std::span<std::uint8_t> bytes,
                           ip::endpoint& endpoint) const;
};

}  // namespace core::ip::udp

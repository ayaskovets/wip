#pragma once

#include "io/fd.hpp"
#include "net/ip/endpoint.hpp"
#include "net/ip/protocol.hpp"
#include "net/ip/version.hpp"

namespace core::net::ip {

class socket : public io::fd {
 protected:
  socket(io::fd fd);

 public:
  socket(net::ip::protocol protocol, net::ip::version version);

 public:
  enum class flag : std::uint8_t {
    kNonblocking,
    kReuseaddr,
    kReuseport,
    kKeepalive,
  };

  void set_flag(flag flag, bool value);
  bool get_flag(flag) const;

 public:
  void bind(const net::ip::endpoint& endpoint);

 public:
  enum class connection_status : std::uint8_t {
    kSuccess,
    kPending,
    kFailure,
  };

  connection_status connect(const net::ip::endpoint& endpoint);

 public:
  net::ip::endpoint get_bind_endpoint() const;
  net::ip::endpoint get_connect_endpoint() const;

 protected:
  std::size_t receive(std::span<std::uint8_t> bytes) const;
  std::size_t send(std::span<const std::uint8_t> bytes) const;

 protected:
  std::size_t receive_from(std::span<std::uint8_t> bytes,
                           net::ip::endpoint& endpoint) const;
  std::size_t send_to(std::span<const std::uint8_t> bytes,
                      const net::ip::endpoint& endpoint) const;
};

}  // namespace core::net::ip

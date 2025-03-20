#pragma once

#include "io/fd.hpp"
#include "ip/endpoint.hpp"
#include "ip/protocol.hpp"
#include "ip/version.hpp"

namespace core::ip {

class socket : public io::fd {
 protected:
  socket(io::fd fd);

 public:
  socket(ip::protocol protocol, ip::version version);

 public:
  enum class flag : std::uint8_t {
    kNonblocking,
    kReuseaddr,
    kReuseport,
    kKeepalive
  };

  void set_flag(flag flag, bool value);
  bool get_flag(flag) const;

 public:
  void bind(const ip::endpoint& endpoint);
  bool connect(const ip::endpoint& endpoint);

 public:
  ip::endpoint get_bind_endpoint() const;
  ip::endpoint get_connect_endpoint() const;
};

}  // namespace core::ip

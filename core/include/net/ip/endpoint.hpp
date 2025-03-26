#pragma once

#include <format>

#include "net/ip/address.hpp"
#include "net/ip/port.hpp"

namespace core::net::ip {

class endpoint final {
 public:
  endpoint(net::ip::address address, net::ip::port port) noexcept;

 public:
  bool operator==(const endpoint& that) const noexcept = default;
  bool operator!=(const endpoint& that) const noexcept = default;

 public:
  const net::ip::address& get_address() const noexcept;
  net::ip::port get_port() const noexcept;

 public:
  std::string to_string() const;

 private:
  net::ip::address address_;
  net::ip::port port_;
};

}  // namespace core::net::ip

template <>
struct std::formatter<core::net::ip::endpoint> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::ip::endpoint& endpoint,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", endpoint.to_string());
  }
};

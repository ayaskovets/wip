#pragma once

#include <format>

#include "ip/address.hpp"
#include "ip/port.hpp"

namespace core::ip {

class endpoint final {
 public:
  endpoint(ip::address address, ip::port port) noexcept;

 public:
  const ip::address& get_address() const noexcept;
  ip::port get_port() const noexcept;

 private:
  ip::address address_;
  ip::port port_;
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::endpoint> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::ip::endpoint& endpoint,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}:{}", endpoint.get_address(),
                          endpoint.get_port());
  }
};

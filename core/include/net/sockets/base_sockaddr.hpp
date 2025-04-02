#pragma once

#include <format>
#include <functional>
#include <memory>
#include <string>

#include "net/sockets/family.hpp"

namespace core::net::sockets {

class base_sockaddr {
 private:
  class storage;

 public:
  // NOTE: base_sockaddr always allocates enough memory to hold a sockaddr of
  // the provided family. It is the responsibility of the derived class to fill
  // the memory as needed
  explicit base_sockaddr(net::sockets::family family);

  // NOTE: this method must not be used anywhere except the subclass
  // constructor, intended only two-step creation of a base_sockaddr object
  storage* get_storage() noexcept;

 public:
  bool operator==(const base_sockaddr& that) const noexcept;
  bool operator!=(const base_sockaddr& that) const noexcept;

 public:
  std::size_t get_length() const;
  net::sockets::family get_family() const;

 public:
  const storage* get_storage() const noexcept;

 public:
 public:
  std::string to_string() const;

 protected:
  std::shared_ptr<storage> storage_;
};

}  // namespace core::net::sockets

template <>
struct std::formatter<core::net::sockets::base_sockaddr> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::net::sockets::base_sockaddr& base_sockaddr,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", base_sockaddr.to_string());
  }
};

#pragma once

#include "ip/protocol.hpp"
#include "ip/version.hpp"

namespace core::ip {

class socket final {
 public:
  socket(ip::version version, ip::protocol protocol) noexcept(false);
  socket(const socket& that) noexcept(false);
  socket& operator=(const socket& that) noexcept(false);
  socket(socket&& that) noexcept;
  socket& operator=(socket&& that) noexcept(false);
  ~socket() noexcept;

 public:
  // NOTE: this method explicitly throws an exception in case of close()
  // failure. This is done to leave RAII semantics for the type but to allow
  // user code to receive the error if it needs to. Any subsequent call results
  // in noop
  void close() noexcept(false);

 private:
  int fd_;
};

}  // namespace core::ip

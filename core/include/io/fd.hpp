#pragma once

#include "utils/static_pimpl.hpp"
#include "utils/tags.hpp"

namespace core::io {

class fd {
 public:
  static const fd& kStdin() noexcept;
  static const fd& kStdout() noexcept;
  static const fd& kStderr() noexcept;

 protected:
  explicit fd(utils::uninitialized_t) noexcept;

 public:
  // NOTE: this class is merely a copy-featured RAII wrapper around a file
  // descriptor. It is assumed that the programmer is responsible for the
  // parameter of the constructor representing an open file descriptor. The
  // constructor still verifies whether the value itself is in the range is a
  // valid file descriptor
  explicit fd(int fd);
  fd(const fd& that);
  fd& operator=(const fd& that);
  fd(fd&& that) noexcept;
  fd& operator=(fd&& that);
  ~fd() noexcept;

 public:
  // NOTE: file descriptor is only equal to itself i.e. operator== is the
  // identity comparison
  bool operator==(const fd& that) const noexcept;
  bool operator!=(const fd& that) const noexcept;

 public:
  // NOTE: this method explicitly throws an exception in case of any close()
  // failure. This is done to leave RAII semantics for the type but to allow
  // user code to receive the error if it needs to. Any subsequent call results
  // in an exception
  void close();

 protected:
  int get_native_handle() const noexcept;

 private:
  struct impl;
  utils::static_pimpl<impl, 4, 4> pimpl_;
};

}  // namespace core::io

#pragma once

namespace core::io {

class fd final {
 public:
  static const fd& kStdin() noexcept;
  static const fd& kStdout() noexcept;
  static const fd& kStderr() noexcept;

 public:
  // NOTE: this class is merely a copy-featured wrapper around a file
  // descriptor. It is assumed that the programmer is responsible for the
  // parameter of the constructor representing a valid file descriptor
  fd(int fd) noexcept;
  fd(const fd& that);
  fd& operator=(const fd& that);
  fd(fd&& that) noexcept;
  fd& operator=(fd&& that);
  ~fd() noexcept;

 public:
  bool operator==(const fd& that) const noexcept;
  bool operator!=(const fd& that) const noexcept;

 public:
  // NOTE: this method explicitly throws an exception in case of any close()
  // failure. This is done to leave RAII semantics for the type but to allow
  // user code to receive the error if it needs to. Any subsequent call results
  // in noop
  void close();

 private:
  int fd_;
};

}  // namespace core::io

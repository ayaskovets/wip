#pragma once

#include <functional>
#include <vector>

#include "io/fd.hpp"
#include "utils/mixins.hpp"
#include "utils/static_pimpl.hpp"

namespace core::io {

// NOTE: poller does not own file descriptors and a close() or another operation
// that invalidates the descriptor would be noticeable only after poll() on the
// invalidated descriptor
class poller final : public utils::non_copyable {
 public:
  enum event : std::uint8_t {
    kPollIn = 1 << 0,
    kPollPri = 1 << 1,
    kPollOut = 1 << 2,
    kPollErr = 1 << 3,
    kPollHup = 1 << 4,
    kPollNval = 1 << 5,
  };
  using events_t = std::uint8_t;
  using callback_t = std::function<void(poller&, const io::fd&, events_t)>;

 public:
  explicit poller(callback_t callback) noexcept;
  poller(poller&&) noexcept;
  poller& operator=(poller&&) noexcept;
  ~poller() noexcept;

 public:
  // NOTE: returns a boolean indicating whether the fd was inserted. Returns
  // false case of an update. Safe to call from the callback
  bool insert_or_assign(const io::fd& fd, events_t events);

  // NOTE: returns a boolean indicating whether the fd was erased. Safe to call
  // from the callback
  bool erase(const io::fd& fd);

 public:
  // NOTE: poll() methods return the number of unhandled errors during the event
  // handling. Any exceptions thrown in the callback function will be silently
  // dropped and accounted in the return value
  std::size_t try_poll(std::chrono::milliseconds timeout);
  std::size_t poll();

 private:
  struct impl;
  utils::static_pimpl<impl, 88, 8> pimpl_;
  callback_t callback_;
};

}  // namespace core::io

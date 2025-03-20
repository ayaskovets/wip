#pragma once

#include <cstdint>

namespace core::ip {

class port final {
 public:
  port(std::uint16_t) noexcept;

 public:
  std::uint16_t as_bytes() const noexcept;

 private:
  std::uint16_t port_;
};

}  // namespace core::ip

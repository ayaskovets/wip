#pragma once

#include <cstdint>

namespace _TMP_::ip {

class port {
 public:
  port(std::uint16_t) noexcept;

 public:
  std::uint16_t as_bytes() const noexcept;

 private:
  std::uint16_t port_;
};

}  // namespace _TMP_::ip

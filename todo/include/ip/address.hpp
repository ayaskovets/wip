#pragma once

#include <string>
#include <vector>

#include "ip/version.hpp"

namespace todo::ip {

class address final {
 public:
  explicit address(std::string address);
  explicit address(std::vector<std::uint8_t> address);

 public:
  bool operator==(const address&) const noexcept = default;

 public:
  version version() const;

 public:
  std::string as_string() const;
  const std::vector<std::uint8_t>& as_bytes() const;

 private:
  std::vector<std::uint8_t> address_;
};

}  // namespace todo::ip

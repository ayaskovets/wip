#pragma once

#include <string>
#include <vector>

namespace todo::ip {

class address final {
 public:
  explicit address(std::string address);
  explicit address(std::vector<std::uint8_t> address);

 public:
  std::string as_string() const;
  const std::vector<std::uint8_t>& as_vector() const;

 private:
  std::vector<std::uint8_t> address_;
};

}  // namespace todo::ip

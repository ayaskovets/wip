#pragma once

#include <array>
#include <ranges>
#include <span>
#include <string>

#include "ip/version.hpp"

namespace core::ip {

class address final {
 private:
  address(const std::uint8_t* data, std::size_t size);

 public:
  explicit address(std::string_view string_view);

  template <std::ranges::range Range>
    requires(std::same_as<std::ranges::range_value_t<Range>, std::uint8_t>)
  constexpr explicit address(Range range)
      : address(std::data(range), std::size(range)) {}

 public:
  bool operator==(const address&) const noexcept;
  constexpr bool operator!=(const address& that) const noexcept {
    return !this->operator==(that);
  }

 public:
  ip::version get_version() const noexcept;

 public:
  std::span<const std::uint8_t> as_bytes() const noexcept;
  std::string as_string() const;

 private:
  std::array<std::uint8_t, 16> data_;
  ip::version version_;
};

}  // namespace core::ip

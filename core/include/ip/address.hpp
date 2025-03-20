#pragma once

#include <array>
#include <format>
#include <ranges>
#include <span>
#include <string>

#include "ip/version.hpp"

namespace core::ip {

class address final {
 private:
  address(const std::uint8_t& data, std::size_t size);

 public:
  explicit address(std::string_view string_view);

  template <std::ranges::contiguous_range Range>
    requires(std::same_as<std::ranges::range_value_t<Range>, std::uint8_t>)
  constexpr explicit address(const Range& range)
      : address(*std::data(range), std::size(range)) {}

 public:
  bool operator==(const address& that) const noexcept;
  constexpr bool operator!=(const address& that) const noexcept {
    return !this->operator==(that);
  }

 public:
  std::span<const std::uint8_t> get_bytes() const noexcept;
  ip::version get_version() const noexcept;

 public:
  std::string to_string() const;

 private:
  std::array<std::uint8_t, 16> data_;
  ip::version version_;
};

}  // namespace core::ip

template <>
struct std::formatter<core::ip::address> {
  template <class FormatContext>
  constexpr auto parse(FormatContext& ctx) const {
    return ctx.begin();
  }

  template <class FormatContext>
  constexpr auto format(const core::ip::address& address,
                        FormatContext& ctx) const {
    return std::format_to(ctx.out(), "{}", address.to_string());
  }
};

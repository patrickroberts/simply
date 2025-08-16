#ifndef SIMPLY_MOVABLE_HPP
#define SIMPLY_MOVABLE_HPP

#include <simply/destructible.hpp>

namespace simply {

struct move_constructible : simply::move_affordance_base {
  template <std::move_constructible T>
  static constexpr auto fn(std::type_identity_t<T> &&self) noexcept(
      std::is_nothrow_move_constructible_v<T>) -> T {
    return std::move(self);
  }
};

struct movable
    : simply::composes<simply::move_constructible, simply::destructible> {};

} // namespace simply

#endif // SIMPLY_MOVABLE_HPP

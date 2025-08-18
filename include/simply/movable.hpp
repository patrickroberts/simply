#ifndef SIMPLY_MOVABLE_HPP
#define SIMPLY_MOVABLE_HPP

#include <simply/type_traits.hpp>

#include <utility>

namespace simply {

struct movable : simply::move_affordance_base {
  template <std::move_constructible T>
  static constexpr auto fn(std::type_identity_t<T> &&self) noexcept(
      std::is_nothrow_move_constructible_v<T>) -> T {
    return std::move(self);
  }
};

} // namespace simply

#endif // SIMPLY_MOVABLE_HPP

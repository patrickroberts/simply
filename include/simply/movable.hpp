#ifndef SIMPLY_MOVABLE_HPP
#define SIMPLY_MOVABLE_HPP

#include <simply/concepts.hpp>

#include <utility>

namespace simply {

struct movable : simply::move_base {
  template <simply::move_constructible T>
  static constexpr auto fn(std::type_identity_t<T> &&self) noexcept(
      simply::nothrow_move_constructible<T>) -> T {
    return std::move(self);
  }
};

} // namespace simply

#endif // SIMPLY_MOVABLE_HPP

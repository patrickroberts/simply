#ifndef SIMPLY_COPYABLE_HPP
#define SIMPLY_COPYABLE_HPP

#include <simply/movable.hpp>

namespace simply {

struct copy_constructible : simply::copy_affordance_base {
  template <typename T>
    requires std::is_copy_constructible_v<T>
  static constexpr auto fn(const std::type_identity_t<T> &self) noexcept(
      std::is_nothrow_copy_constructible_v<T>) -> T {
    return self;
  }
};

struct copyable
    : simply::composes<simply::copy_constructible, simply::movable> {};

} // namespace simply

#endif // SIMPLY_COPYABLE_HPP

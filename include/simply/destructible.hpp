#ifndef SIMPLY_DESTRUCTIBLE_HPP
#define SIMPLY_DESTRUCTIBLE_HPP

#include <simply/type_traits.hpp>

#include <memory>

namespace simply {

struct destructible : simply::destroy_affordance_base {
  template <std::destructible T>
  static constexpr void fn(std::type_identity_t<T> &self) noexcept {
    if constexpr (not std::is_trivially_destructible_v<T>) {
      std::destroy_at(std::addressof(self));
    }
  }
};

} // namespace simply

#endif // SIMPLY_DESTRUCTIBLE_HPP

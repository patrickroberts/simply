#ifndef SIMPLY_COPYABLE_HPP
#define SIMPLY_COPYABLE_HPP

#include <simply/concepts.hpp>

namespace simply {

struct copyable : simply::copy_base {
  template <simply::copy_constructible T>
  static constexpr auto fn(const std::type_identity_t<T> &self) noexcept(
      simply::nothrow_copy_constructible<T>) -> T {
    return self;
  }
};

} // namespace simply

#endif // SIMPLY_COPYABLE_HPP

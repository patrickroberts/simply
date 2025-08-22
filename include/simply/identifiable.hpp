#ifndef SIMPLY_IDENTIFIABLE_HPP
#define SIMPLY_IDENTIFIABLE_HPP

#include <simply/type_traits.hpp>

#include <typeinfo>

namespace simply {

struct identifiable : simply::member_base {
  template <typename T>
  static constexpr auto fn(const std::type_identity_t<T> &value)
      -> const std::type_info & {
    return typeid(T);
  }
};

} // namespace simply

#endif // SIMPLY_IDENTIFIABLE_HPP

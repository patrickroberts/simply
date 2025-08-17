#ifndef SIMPLY_IMPL_HPP
#define SIMPLY_IMPL_HPP

#include <simply/concepts.hpp>

#include <utility>

namespace simply {

template <typename Affordance, typename T, typename Fn>
struct impl {
  static constexpr auto fn = Affordance::template fn<T>;
};

template <typename Affordance, typename T>
inline constexpr const auto &fn = simply::impl<Affordance, T>::fn;

// TODO consider `bind<Affordance>(self)(rest...)` as an alternative to
// `fn<Affordance, remove_cvref_t<decltype(self)>>(self, rest...)` that exposes
// operator() as an overload set with non-deduced parameter types

template <simply::member_affordance Affordance, typename T>
struct affordance_traits<Affordance, T> {
  using function_type = decltype(Affordance::template fn<T>);
};

// allows impl<impl<A, T>, Dyn>::fn to implement A for Dyn by type erasing T
template <typename Affordance, typename T, typename Dyn>
struct affordance_traits<simply::impl<Affordance, T>, Dyn>
    : affordance_traits<Affordance, Dyn> {};

} // namespace simply

#endif // SIMPLY_IMPL_HPP

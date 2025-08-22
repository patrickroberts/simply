#ifndef SIMPLY_VTABLE_HPP
#define SIMPLY_VTABLE_HPP

#include <simply/iface.hpp>
#include <simply/impl.hpp>

namespace simply {

template <simply::affordance Affordance, typename Self>
struct vtable : simply::vtable<simply::unique_fundamental_t<Affordance>, Self> {
};

template <simply::fundamental... Affordances, typename Self>
struct vtable<simply::composes<Affordances...>, Self>
    : simply::vtable<Affordances, Self>... {};

template <simply::fundamental Affordance, typename Self>
struct vtable<Affordance, Self> {};

template <simply::member Affordance, typename Self>
struct vtable<Affordance, Self> {
  simply::function_type_t<Affordance, Self> *fn;
};

// TODO specialize affordance_traits so this is unnecessary
template <simply::constructor Affordance, typename Self>
struct vtable<Affordance, Self> {
  using iface_type = simply::iface<typename Self::storage_type, Self>;
  simply::function_type_t<Affordance, iface_type> *fn;
};

template <simply::affordance Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self> vtable_for = {
    simply::vtable_for<simply::unique_fundamental_t<Affordance>, Self, T>,
};

template <simply::fundamental... Affordances, typename Self, typename T>
inline constexpr simply::vtable<simply::composes<Affordances...>, Self>
    vtable_for<simply::composes<Affordances...>, Self, T> = {
        simply::vtable_for<Affordances, Self, T>...,
};

template <simply::fundamental Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self>
    vtable_for<Affordance, Self, T>{};

template <simply::member Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self>
    vtable_for<Affordance, Self, T> = {
        .fn = &simply::fn<simply::impl<Affordance, T>, Self>,
};

// TODO specialize impl so this is unnecessary
template <simply::constructor Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self>
    vtable_for<Affordance, Self, T> = {
        .fn = &simply::fn<simply::impl<Affordance, T>,
                          simply::iface<typename Self::storage_type, Self>>,
};

} // namespace simply

#endif // SIMPLY_VTABLE_HPP

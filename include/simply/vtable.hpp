#ifndef SIMPLY_VTABLE_HPP
#define SIMPLY_VTABLE_HPP

#include <simply/impl.hpp>

namespace simply {

template <simply::affordance Affordance, typename Self>
struct vtable
    : simply::vtable<simply::unique_fundamental_affordances_t<Affordance>,
                     Self> {};

template <simply::fundamental_affordance... Affordances, typename Self>
struct vtable<simply::conjunction<Affordances...>, Self>
    : simply::vtable<Affordances, Self>... {};

template <simply::fundamental_affordance Affordance, typename Self>
struct vtable<Affordance, Self> {};

template <simply::member_affordance Affordance, typename Self>
struct vtable<Affordance, Self> {
  simply::affordance_traits<Affordance, Self>::function_type *fn;
};

template <simply::affordance Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self> vtable_for = {
    simply::vtable_for<simply::unique_fundamental_affordances_t<Affordance>,
                       Self, T>,
};

template <simply::fundamental_affordance... Affordances, typename Self,
          typename T>
inline constexpr simply::vtable<simply::conjunction<Affordances...>, Self>
    vtable_for<simply::conjunction<Affordances...>, Self, T> = {
        simply::vtable_for<Affordances, Self, T>...,
};

template <simply::fundamental_affordance Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self>
    vtable_for<Affordance, Self, T>{};

template <simply::member_affordance Affordance, typename Self, typename T>
inline constexpr simply::vtable<Affordance, Self>
    vtable_for<Affordance, Self, T> = {
        .fn = &simply::impl<simply::impl<Affordance, T>, Self>::fn,
};

} // namespace simply

#endif // SIMPLY_VTABLE_HPP

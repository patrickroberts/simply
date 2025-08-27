#ifndef SIMPLY_VTABLE_HPP
#define SIMPLY_VTABLE_HPP

#include <simply/iface.hpp>
#include <simply/impl.hpp>

namespace simply {

template <simply::mixin Mixin, typename Self>
struct vtable : simply::vtable<simply::unique_fundamental_t<Mixin>, Self> {};

template <simply::fundamental... Mixins, typename Self>
struct vtable<simply::composes<Mixins...>, Self>
    : simply::vtable<Mixins, Self>... {};

template <simply::fundamental Mixin, typename Self>
struct vtable<Mixin, Self> {};

template <simply::member Mixin, typename Self>
struct vtable<Mixin, Self> {
  simply::function_type_t<Mixin, Self> *fn;
};

// TODO specialize mixin_traits so this is unnecessary
template <simply::constructor Mixin, typename Self>
struct vtable<Mixin, Self> {
  using iface_type = simply::iface<typename Self::storage_type, Self>;
  simply::function_type_t<Mixin, iface_type> *fn;
};

template <simply::mixin Mixin, typename Self, typename T>
inline constexpr simply::vtable<Mixin, Self> vtable_for = {
    simply::vtable_for<simply::unique_fundamental_t<Mixin>, Self, T>,
};

template <simply::fundamental... Mixins, typename Self, typename T>
inline constexpr simply::vtable<simply::composes<Mixins...>, Self>
    vtable_for<simply::composes<Mixins...>, Self, T> = {
        simply::vtable_for<Mixins, Self, T>...,
};

template <simply::fundamental Mixin, typename Self, typename T>
inline constexpr simply::vtable<Mixin, Self> vtable_for<Mixin, Self, T>{};

template <simply::member Mixin, typename Self, typename T>
inline constexpr simply::vtable<Mixin, Self> vtable_for<Mixin, Self, T> = {
    .fn = &simply::fn<simply::impl<Mixin, T>, Self>,
};

// TODO specialize impl so this is unnecessary
template <simply::constructor Mixin, typename Self, typename T>
inline constexpr simply::vtable<Mixin, Self> vtable_for<Mixin, Self, T> = {
    .fn = &simply::fn<simply::impl<Mixin, T>,
                      simply::iface<typename Self::storage_type, Self>>,
};

} // namespace simply

#endif // SIMPLY_VTABLE_HPP

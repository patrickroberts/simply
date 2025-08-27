#ifndef SIMPLY_IFACE_HPP
#define SIMPLY_IFACE_HPP

#include <simply/concepts.hpp>

namespace simply {

template <simply::mixin Mixin, typename Self>
struct iface {};

template <simply::composition Mixin, typename Self>
struct iface<Mixin, Self>
    : simply::iface<simply::unique_fundamental_t<Mixin>, Self> {};

template <simply::fundamental... Mixins, typename Self>
struct iface<simply::composes<Mixins...>, Self>
    : simply::iface<Mixins, Self>... {};

} // namespace simply

#endif // SIMPLY_IFACE_HPP

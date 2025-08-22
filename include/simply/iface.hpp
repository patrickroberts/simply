#ifndef SIMPLY_IFACE_HPP
#define SIMPLY_IFACE_HPP

#include <simply/concepts.hpp>

namespace simply {

template <simply::affordance Affordance, typename Self>
struct iface {};

template <simply::composition Affordance, typename Self>
struct iface<Affordance, Self>
    : simply::iface<simply::unique_fundamental_t<Affordance>, Self> {};

template <simply::fundamental... Affordances, typename Self>
struct iface<simply::composes<Affordances...>, Self>
    : simply::iface<Affordances, Self>... {};

} // namespace simply

#endif // SIMPLY_IFACE_HPP

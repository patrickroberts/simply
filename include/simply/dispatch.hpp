#ifndef SIMPLY_DISPATCH_HPP
#define SIMPLY_DISPATCH_HPP

#include <simply/iface.hpp>
#include <simply/vtable.hpp>

#include <memory>

namespace simply {

struct indirect_dispatch : simply::dispatch_affordance_base {
  template <simply::affordance Affordance>
  struct type : simply::affordance_base {
    using affordance_type = Affordance;
  };
};

struct inplace_dispatch : simply::dispatch_affordance_base {
  template <simply::affordance Affordance>
  struct type : simply::affordance_base {
    using affordance_type = Affordance;
  };
};

template <simply::specialization_of<simply::indirect_dispatch::type> Dispatch,
          typename Self>
  requires simply::affordance<Dispatch>
struct iface<Dispatch, Self> {
  template <typename T>
  constexpr iface([[maybe_unused]] std::in_place_type_t<T> tag)
      : vtable_ptr(
            std::addressof(simply::vtable_for<affordance_type, Self, T>)) {}

private:
  template <typename Affordance, typename Dyn, typename Fn>
  friend struct simply::impl;

  template <simply::member_affordance Member>
  [[nodiscard]] constexpr auto
  get_member([[maybe_unused]] std::in_place_type_t<Member> tag) const noexcept {
    using base_type = simply::vtable<Member, Self>;
    static_assert(std::derived_from<vtable_type, base_type>);
    // disambiguates base class
    return static_cast<const base_type &>(*vtable_ptr).fn;
  }

  using affordance_type = Dispatch::affordance_type;
  using vtable_type = simply::vtable<affordance_type, Self>;

  const vtable_type *vtable_ptr;
};

template <simply::specialization_of<simply::inplace_dispatch::type> Dispatch,
          typename Self>
  requires simply::affordance<Dispatch>
struct iface<Dispatch, Self> {
  template <typename T>
  constexpr iface([[maybe_unused]] std::in_place_type_t<T> tag)
      : vtable(simply::vtable_for<affordance_type, Self, T>) {}

private:
  template <typename Affordance, typename Dyn, typename Fn>
  friend struct simply::impl;

  template <simply::member_affordance Member>
  [[nodiscard]] constexpr auto
  get_member([[maybe_unused]] std::in_place_type_t<Member> tag) const noexcept {
    using base_type = simply::vtable<Member, Self>;
    static_assert(std::derived_from<vtable_type, base_type>);
    // disambiguates base class
    return static_cast<const base_type &>(vtable).fn;
  }

  using affordance_type = Dispatch::affordance_type;
  using vtable_type = simply::vtable<affordance_type, Self>;

  vtable_type vtable;
};

} // namespace simply

#endif // SIMPLY_DISPATCH_HPP

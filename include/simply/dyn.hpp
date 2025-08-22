#ifndef SIMPLY_DYN_HPP
#define SIMPLY_DYN_HPP

#include <simply/concepts.hpp>
#include <simply/dispatch.hpp>
#include <simply/iface.hpp>
#include <simply/impl.hpp>
#include <simply/storage.hpp>

#include <memory>

namespace simply {

// TODO reconsider template parameters to avoid requiring user to explicitly
// categorize affordances, or provide a type alias template to categorize them
// and inject defaults. Rename to `basic_dyn`?
template <simply::affordance Affordance,
          simply::storage Storage = simply::allocator_storage<>,
          simply::dispatch Dispatch = simply::indirect_dispatch>
class dyn
    : public simply::iface<Affordance,
                           simply::dyn<Affordance, Storage, Dispatch>>,
      public simply::iface<Storage, simply::dyn<Affordance, Storage, Dispatch>>,
      public simply::iface<Dispatch,
                           simply::dyn<Affordance, Storage, Dispatch>> {
  using storage_base = simply::iface<Storage, dyn>;
  using dispatch_base = simply::iface<Dispatch, dyn>;

public:
  using affordance_type = Affordance;
  using storage_type = Storage;
  using dispatch_type = Dispatch;

  constexpr dyn(const dyn &other)
    requires simply::copy_affordance<Affordance>
  = default;

  constexpr dyn(dyn &&other) noexcept = default;

  template <typename Alloc, simply::affords<Affordance> T, typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr explicit dyn(std::allocator_arg_t alloc_tag, const Alloc &alloc,
                         std::in_place_type_t<T> obj_tag, Args &&...args)
      : storage_base(alloc_tag, alloc, obj_tag, std::forward<Args>(args)...),
        dispatch_base(obj_tag) {}

  template <simply::affords<Affordance> T, typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr explicit dyn(std::in_place_type_t<T> tag, Args &&...args)
      : storage_base(tag, std::forward<Args>(args)...), dispatch_base(tag) {}

  template <typename T>
    requires(not std::same_as<std::remove_cvref_t<T>, dyn>) and
                (not simply::specialization_of<std::remove_cvref_t<T>,
                                               std::in_place_type_t>) and
                simply::affords<std::decay_t<T>, Affordance>
  constexpr explicit dyn(T &&value)
      : storage_base(std::in_place_type<std::decay_t<T>>,
                     std::forward<T>(value)),
        dispatch_base(std::in_place_type<std::decay_t<T>>) {}

  constexpr auto operator=(const dyn &other) -> dyn &
    requires simply::copy_affordance<Affordance>
  {
    // prevent self-assignment
    if (this == std::addressof(other)) {
      return *this;
    }

    std::destroy_at(this);
    std::construct_at(this, other);
    return *this;
  }

  constexpr auto operator=(dyn &&other) noexcept -> dyn & {
    std::destroy_at(this);
    std::construct_at(this, std::move(other));
    return *this;
  }

  // TODO expose overloads for assignment affordances

  constexpr ~dyn()
    requires simply::destroy_affordance<Affordance>
  {
    using destroy = simply::destroy_affordance_t<Affordance>;
    simply::fn<destroy, dyn>(*this);
  }
};

namespace pmr {

using polymorphic_allocator_storage =
    simply::allocator_storage<std::pmr::polymorphic_allocator<std::byte>>;

template <simply::affordance Affordance,
          simply::dispatch Dispatch = simply::indirect_dispatch>
using dyn = simply::dyn<Affordance, simply::pmr::polymorphic_allocator_storage,
                        Dispatch>;

} // namespace pmr

template <typename Affordance, typename Storage, typename Dispatch>
struct affordance_type<simply::dyn<Affordance, Storage, Dispatch>> {
  using type = Affordance;
};

// dynamic dispatch for member affordances of dyn
template <simply::member Affordance, simply::specialization_of<simply::dyn> Dyn,
          typename R, typename Self, typename... Args, bool NoExcept>
struct impl<Affordance, Dyn, R(Self, Args...) noexcept(NoExcept)> {
  static constexpr auto fn(Self dyn, Args... args) noexcept(NoExcept) -> R {
    const auto fn = dyn.get_member(std::in_place_type<Affordance>);
    return fn(std::forward<Self>(dyn), std::forward<Args>(args)...);
  }
};

// TODO refactor specializations below to not assume allocator_storage
template <typename Dyn>
concept _allocator_storage_dyn =
    simply::specialization_of<Dyn, simply::dyn> and
    simply::specialization_of<typename Dyn::storage_type,
                              simply::allocator_storage>;

// only copy the storage subobject of Dyn
template <simply::_allocator_storage_dyn Dyn>
struct affordance_traits<simply::copyable, Dyn> {
  using function_type =
      simply::iface<typename Dyn::storage_type, Dyn>(const Dyn &);
};

template <simply::member Affordance, typename T,
          simply::_allocator_storage_dyn Dyn, typename R, typename Self,
          typename... Args, bool NoExcept>
struct impl<simply::impl<Affordance, T>, Dyn,
            R(Self, Args...) noexcept(NoExcept)> {
  static constexpr auto fn(Self dyn, Args... args) noexcept(NoExcept) -> R {
    using self_type = simply::apply_cvref_t<Self, T>;

    const auto pointer = simply::_dispatch_fn<T, Dyn>(dyn);
    return simply::fn<Affordance, T>(std::forward<self_type>(*pointer),
                                     std::forward<Args>(args)...);
  }
};

// TODO should this be specialized for simply::destructible only?
template <simply::fundamental_destroy_affordance Affordance, typename T,
          simply::_allocator_storage_dyn Dyn, typename R, typename Self,
          typename... Args, bool NoExcept>
struct impl<simply::impl<Affordance, T>, Dyn,
            R(Self, Args...) noexcept(NoExcept)> {
  static_assert(
      std::same_as<R(Self, Args...) noexcept(NoExcept), void(Dyn &) noexcept>);

  static constexpr void fn(Dyn &dyn) noexcept {
    if (dyn.valueless_after_move()) {
      return;
    }

    auto alloc = simply::_rebind_alloc<T>(dyn.get_allocator());
    const auto pointer = simply::_dispatch_fn<T, Dyn>(dyn);

    using traits = std::allocator_traits<decltype(alloc)>;

    traits::destroy(alloc, std::to_address(pointer));
    traits::deallocate(alloc, pointer, 1);
  }
};

// allocator_storage does not need vtable entry for move affordance
template <simply::fundamental_move_affordance Affordance,
          simply::_allocator_storage_dyn Dyn>
struct vtable<Affordance, Dyn> {};

template <simply::fundamental_move_affordance Affordance,
          simply::_allocator_storage_dyn Dyn, typename T>
inline constexpr simply::vtable<Affordance, Dyn>
    vtable_for<Affordance, Dyn, T>{};

} // namespace simply

#endif // SIMPLY_DYN_HPP

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
// categorize mixins, or provide a type alias template to categorize them
// and inject defaults. Rename to `basic_dyn`?
template <simply::mixin Mixin,
          simply::storage Storage = simply::allocator_storage<>,
          simply::dispatch Dispatch = simply::indirect_dispatch>
class dyn
    : public simply::iface<Mixin, simply::dyn<Mixin, Storage, Dispatch>>,
      public simply::iface<Storage, simply::dyn<Mixin, Storage, Dispatch>>,
      public simply::iface<Dispatch, simply::dyn<Mixin, Storage, Dispatch>> {
  using storage_base = simply::iface<Storage, dyn>;
  using dispatch_base = simply::iface<Dispatch, dyn>;

public:
  using mixin_type = Mixin;
  using storage_type = Storage;
  using dispatch_type = Dispatch;

  // TODO add default constructor for nullable mixin

  // TODO generalize member constraints based on storage
  constexpr dyn(const dyn &other)
    requires simply::copy_mixin<Mixin>
  = default;

  constexpr dyn(dyn &&other) noexcept = default;

  template <typename Alloc, simply::has_mixin<Mixin> T, typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr explicit dyn(std::allocator_arg_t alloc_tag, const Alloc &alloc,
                         std::in_place_type_t<T> obj_tag, Args &&...args)
      : storage_base(alloc_tag, alloc, obj_tag, std::forward<Args>(args)...),
        dispatch_base(obj_tag) {}

  template <simply::has_mixin<Mixin> T, typename... Args>
    requires std::constructible_from<T, Args...>
  constexpr explicit dyn(std::in_place_type_t<T> tag, Args &&...args)
      : storage_base(tag, std::forward<Args>(args)...), dispatch_base(tag) {}

  template <typename T>
    requires(not std::same_as<std::remove_cvref_t<T>, dyn>) and
                (not simply::specialization_of<std::remove_cvref_t<T>,
                                               std::in_place_type_t>) and
                simply::has_mixin<std::decay_t<T>, Mixin>
  constexpr explicit dyn(T &&value)
      : storage_base(std::in_place_type<std::decay_t<T>>,
                     std::forward<T>(value)),
        dispatch_base(std::in_place_type<std::decay_t<T>>) {}

  constexpr auto operator=(const dyn &other) -> dyn &
    requires simply::copy_mixin<Mixin>
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

  // TODO expose overloads for assignment mixins

  constexpr ~dyn()
    requires simply::destroy_mixin<Mixin>
  {
    using destroy = simply::destroy_mixin_t<Mixin>;
    simply::fn<destroy, dyn>(*this);
  }
};

namespace pmr {

using polymorphic_allocator_storage =
    simply::allocator_storage<std::pmr::polymorphic_allocator<std::byte>>;

template <simply::mixin Mixin,
          simply::dispatch Dispatch = simply::indirect_dispatch>
using dyn =
    simply::dyn<Mixin, simply::pmr::polymorphic_allocator_storage, Dispatch>;

} // namespace pmr

template <typename Mixin, typename Storage, typename Dispatch>
struct mixin_type<simply::dyn<Mixin, Storage, Dispatch>> {
  using type = Mixin;
};

// dynamic dispatch for member mixins of dyn
template <simply::member Mixin, simply::specialization_of<simply::dyn> Dyn,
          typename R, typename Self, typename... Args, bool NoExcept>
struct impl<Mixin, Dyn, R(Self, Args...) noexcept(NoExcept)> {
  static constexpr auto fn(Self dyn, Args... args) noexcept(NoExcept) -> R {
    const auto fn = dyn.get_member(std::in_place_type<Mixin>);
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
struct mixin_traits<simply::copyable, Dyn> {
  using function_type =
      simply::iface<typename Dyn::storage_type, Dyn>(const Dyn &);
};

template <simply::member Mixin, typename T, simply::_allocator_storage_dyn Dyn,
          typename R, typename Self, typename... Args, bool NoExcept>
struct impl<simply::impl<Mixin, T>, Dyn, R(Self, Args...) noexcept(NoExcept)> {
  static constexpr auto fn(Self dyn, Args... args) noexcept(NoExcept) -> R {
    using self_type = simply::apply_cvref_t<Self, T>;

    const auto pointer = simply::_dispatch_fn<T, Dyn>(dyn);
    return simply::fn<Mixin, T>(std::forward<self_type>(*pointer),
                                std::forward<Args>(args)...);
  }
};

// TODO should this be specialized for simply::destructible only?
template <simply::fundamental_destroy_mixin Mixin, typename T,
          simply::_allocator_storage_dyn Dyn, typename R, typename Self,
          typename... Args, bool NoExcept>
struct impl<simply::impl<Mixin, T>, Dyn, R(Self, Args...) noexcept(NoExcept)> {
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

// allocator_storage does not need vtable entry for move mixin
template <simply::fundamental_move_mixin Mixin,
          simply::_allocator_storage_dyn Dyn>
struct vtable<Mixin, Dyn> {};

template <simply::fundamental_move_mixin Mixin,
          simply::_allocator_storage_dyn Dyn, typename T>
inline constexpr simply::vtable<Mixin, Dyn> vtable_for<Mixin, Dyn, T>{};

} // namespace simply

#endif // SIMPLY_DYN_HPP

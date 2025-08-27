#ifndef SIMPLY_TYPE_TRAITS_HPP
#define SIMPLY_TYPE_TRAITS_HPP

#include <concepts>
#include <type_traits>

namespace simply {

struct mixin_base {};

struct compound_base : simply::mixin_base {};

struct member_base : simply::mixin_base {};

struct constructor_base : simply::member_base {};

struct copy_base : simply::constructor_base {};

struct move_base : simply::constructor_base {};

struct destroy_base : simply::member_base {};

struct storage_base : simply::member_base {};

struct dispatch_base : simply::mixin_base {};

template <typename Mixin, typename T>
struct mixin_traits;

template <typename Mixin, typename T>
using function_type_t = simply::mixin_traits<Mixin, T>::function_type;

template <typename T>
struct mixin_type;

template <typename T>
using mixin_type_t = simply::mixin_type<T>::type;

template <typename Mixin, typename T,
          typename Fn = simply::function_type_t<Mixin, T>>
struct impl;

template <typename Mixin, typename Tag>
struct fundamental_type {};

template <typename Mixin, typename Tag>
using fundamental_type_t = simply::fundamental_type<Mixin, Tag>::type;

template <typename Mixin>
using copy_mixin_t = simply::fundamental_type_t<Mixin, simply::copy_base>;

template <typename Mixin>
using move_mixin_t = simply::fundamental_type_t<Mixin, simply::move_base>;

template <typename Mixin>
using destroy_mixin_t = simply::fundamental_type_t<Mixin, simply::destroy_base>;

template <typename... Ts>
struct composes : simply::compound_base {};

template <typename... Ts>
struct chooses : simply::compound_base {};

template <typename, template <typename...> typename>
inline constexpr bool is_specialization_of_v = false;

template <typename... Ts, template <typename...> typename F>
inline constexpr bool is_specialization_of_v<F<Ts...>, F> = true;

template <template <typename...> typename Base, typename... Ts>
auto _as_specialization_of(Base<Ts...> *) -> Base<Ts...>;

template <typename Derived, template <typename...> typename Base>
using base_specialization_of_t = decltype(simply::_as_specialization_of<Base>(
    static_cast<Derived *>(nullptr)));

template <typename T>
using base_composition_t =
    simply::base_specialization_of_t<T, simply::composes>;

template <typename T>
inline constexpr bool enable_mixin = std::derived_from<T, simply::mixin_base>;

template <typename Mixin, typename T>
inline constexpr bool enable_mixin_for =
    requires { simply::impl<Mixin, T>::fn; };

template <typename T, typename Tag>
inline constexpr bool enable_mixin_tag = std::derived_from<T, Tag>;

template <typename Mixin, typename Unique = simply::composes<>>
struct unique_fundamental;

template <typename Mixin>
using unique_fundamental_t = simply::unique_fundamental<Mixin>::type;

template <typename From, typename To>
struct apply_cvref : std::remove_cvref<To> {};

template <typename From, typename To>
using apply_cvref_t = simply::apply_cvref<From, To>::type;

template <typename From, typename To>
struct apply_cvref<From &, To>
    : std::add_lvalue_reference<simply::apply_cvref_t<From, To>> {};

template <typename From, typename To>
struct apply_cvref<From &&, To>
    : std::add_rvalue_reference<simply::apply_cvref_t<From, To>> {};

template <typename From, typename To>
struct apply_cvref<const From, To> : std::add_const<std::remove_cvref_t<To>> {};

template <typename From, typename To>
struct apply_cvref<volatile From, To>
    : std::add_volatile<std::remove_cvref_t<To>> {};

template <typename From, typename To>
struct apply_cvref<const volatile From, To>
    : std::add_cv<std::remove_cvref_t<To>> {};

} // namespace simply

#endif // SIMPLY_TYPE_TRAITS_HPP

#ifndef SIMPLY_TYPE_TRAITS_HPP
#define SIMPLY_TYPE_TRAITS_HPP

#include <concepts>
#include <type_traits>

namespace simply {

struct affordance_base {};

struct member_affordance_base : simply::affordance_base {};

struct constructor_affordance_base : simply::member_affordance_base {};

struct copy_affordance_base : simply::constructor_affordance_base {};

struct move_affordance_base : simply::constructor_affordance_base {};

struct destroy_affordance_base : simply::member_affordance_base {};

struct storage_affordance_base : simply::member_affordance_base {};

struct dispatch_affordance_base : simply::affordance_base {};

template <typename Affordance, typename T>
struct affordance_traits;

template <typename Affordance, typename Tag>
struct fundamental_affordance_type {};

template <typename Affordance, typename Tag>
using fundamental_affordance_type_t =
    simply::fundamental_affordance_type<Affordance, Tag>::type;

template <typename Affordance>
using copy_affordance_t =
    simply::fundamental_affordance_type_t<Affordance,
                                          simply::copy_affordance_base>;

template <typename Affordance>
using move_affordance_t =
    simply::fundamental_affordance_type_t<Affordance,
                                          simply::move_affordance_base>;

template <typename Affordance>
using destroy_affordance_t =
    simply::fundamental_affordance_type_t<Affordance,
                                          simply::destroy_affordance_base>;

template <typename... Ts>
struct conjunction : simply::affordance_base {};

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
using base_conjunction_t =
    simply::base_specialization_of_t<T, simply::conjunction>;

template <typename T>
inline constexpr bool enable_affordance =
    std::derived_from<T, simply::affordance_base>;

template <typename Affordance, typename T>
inline constexpr bool enable_affordance_for =
    requires { &Affordance::template fn<T>; };

template <typename T, typename Tag>
inline constexpr bool enable_affordance_tag = std::derived_from<T, Tag>;

template <typename Affordance, typename Unique = simply::conjunction<>>
struct unique_fundamental_affordances;

template <typename Affordance>
using unique_fundamental_affordances_t =
    simply::unique_fundamental_affordances<Affordance>::type;

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

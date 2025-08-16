#ifndef SIMPLY_CONCEPTS_HPP
#define SIMPLY_CONCEPTS_HPP

#include <simply/type_traits.hpp>

namespace simply {

template <typename T>
concept affordance = simply::enable_affordance<T>;

template <typename T>
concept copy_affordance =
    simply::affordance<T> and
    simply::enable_affordance_tag<T, simply::copy_affordance_base>;

template <typename T>
concept move_affordance =
    simply::affordance<T> and
    simply::enable_affordance_tag<T, simply::move_affordance_base>;

template <typename T>
concept destroy_affordance =
    simply::affordance<T> and
    simply::enable_affordance_tag<T, simply::destroy_affordance_base>;

template <typename T>
concept storage_affordance =
    simply::affordance<T> and
    simply::enable_affordance_tag<T, simply::storage_affordance_base>;

template <typename T>
concept dispatch_affordance =
    simply::affordance<T> and
    simply::enable_affordance_tag<T, simply::dispatch_affordance_base>;

template <typename Affordance, typename T>
concept affordance_for = simply::affordance<Affordance> and
                         simply::enable_affordance_for<Affordance, T>;

template <typename T, typename Affordance>
concept affords = simply::affordance_for<Affordance, T>;

template <typename T, template <typename...> typename F>
concept specialization_of = simply::is_specialization_of_v<T, F>;

template <typename Derived, template <typename...> typename Base>
concept derived_from_specialization_of = requires(Derived *derived) {
  simply::_as_specialization_of<Base>(derived);
};

template <typename T>
concept compound_affordance =
    simply::affordance<T> and
    std::derived_from<T, simply::compound_affordance_base>;

template <typename T>
concept fundamental_affordance =
    simply::affordance<T> and not simply::compound_affordance<T>;

template <typename T>
concept composition_affordance =
    simply::compound_affordance<T> and
    simply::derived_from_specialization_of<T, simply::composes>;

template <typename T>
concept choice_affordance =
    simply::compound_affordance<T> and
    simply::derived_from_specialization_of<T, simply::chooses>;

template <typename T>
concept member_affordance =
    simply::fundamental_affordance<T> and
    simply::enable_affordance_tag<T, simply::member_affordance_base>;

template <typename T>
concept constructor_affordance =
    simply::member_affordance<T> and
    simply::enable_affordance_tag<T, simply::constructor_affordance_base>;

template <typename T>
concept fundamental_copy_affordance =
    simply::constructor_affordance<T> and
    simply::enable_affordance_tag<T, simply::copy_affordance_base>;

template <typename T>
concept fundamental_move_affordance =
    simply::constructor_affordance<T> and
    simply::enable_affordance_tag<T, simply::move_affordance_base>;

template <typename T>
concept fundamental_destroy_affordance =
    simply::member_affordance<T> and
    simply::enable_affordance_tag<T, simply::destroy_affordance_base>;

template <typename T, typename U>
concept different_from = not std::same_as<T, U>;

template <simply::composition_affordance Composition, typename T>
inline constexpr bool enable_affordance_for<Composition, T> =
    simply::enable_affordance_for<
        simply::unique_fundamental_affordances_t<Composition>, T>;

template <simply::fundamental_affordance... Fundamental, typename T>
inline constexpr bool
    enable_affordance_for<simply::composes<Fundamental...>, T> =
        (... and simply::enable_affordance_for<Fundamental, T>);

template <simply::composition_affordance T, typename Tag>
inline constexpr bool enable_affordance_tag<T, Tag> =
    simply::enable_affordance_tag<simply::unique_fundamental_affordances_t<T>,
                                  Tag>;

template <simply::fundamental_affordance... Ts, typename Tag>
inline constexpr bool enable_affordance_tag<simply::composes<Ts...>, Tag> =
    (... or simply::enable_affordance_tag<Ts, Tag>);

template <simply::composition_affordance Composition, typename Tag>
struct fundamental_affordance_type<Composition, Tag>
    : simply::fundamental_affordance_type<
          simply::unique_fundamental_affordances_t<Composition>, Tag> {};

template <simply::fundamental_affordance... Fundamental, typename Tag>
struct fundamental_affordance_type<simply::composes<Fundamental...>, Tag>
    : simply::fundamental_affordance_type<Fundamental, Tag>... {};

template <simply::fundamental_affordance Affordance, typename Tag>
  requires simply::enable_affordance_tag<Affordance, Tag>
struct fundamental_affordance_type<Affordance, Tag>
    : std::type_identity<Affordance> {};

// terminate on outer composes<>
template <typename Unique>
struct unique_fundamental_affordances<simply::composes<>, Unique>
    : std::type_identity<Unique> {};

// handle outer Fundamental as composes<Fundamental>
template <simply::fundamental_affordance Fundamental, typename Unique>
struct unique_fundamental_affordances<Fundamental, Unique>
    : simply::unique_fundamental_affordances<simply::composes<Fundamental>,
                                             Unique> {};

// handle outer Composition as composes<...>
template <simply::composition_affordance Composition, typename Unique>
struct unique_fundamental_affordances<Composition, Unique>
    : simply::unique_fundamental_affordances<
          simply::base_composition_t<Composition>, Unique> {};

// add inner Fundamental if different from all Unique
template <simply::fundamental_affordance Fundamental, typename... Rest,
          simply::different_from<Fundamental>... Unique>
struct unique_fundamental_affordances<simply::composes<Fundamental, Rest...>,
                                      simply::composes<Unique...>>
    : simply::unique_fundamental_affordances<
          simply::composes<Rest...>, simply::composes<Unique..., Fundamental>> {
};

// drop inner Fundamental otherwise
template <simply::fundamental_affordance Fundamental, typename... Rest,
          typename Unique>
struct unique_fundamental_affordances<simply::composes<Fundamental, Rest...>,
                                      Unique>
    : simply::unique_fundamental_affordances<simply::composes<Rest...>,
                                             Unique> {};

// handle inner Composition as composes<...>
template <simply::composition_affordance Composition, typename... Rest,
          typename Unique>
struct unique_fundamental_affordances<simply::composes<Composition, Rest...>,
                                      Unique>
    : simply::unique_fundamental_affordances<
          simply::composes<simply::base_composition_t<Composition>, Rest...>,
          Unique> {};

// handle inner composes<First...> as First...
template <typename... First, typename... Rest, typename Unique>
struct unique_fundamental_affordances<
    simply::composes<simply::composes<First...>, Rest...>, Unique>
    : simply::unique_fundamental_affordances<
          simply::composes<First..., Rest...>, Unique> {};

} // namespace simply

#endif // SIMPLY_CONCEPTS_HPP

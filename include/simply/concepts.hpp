#ifndef SIMPLY_CONCEPTS_HPP
#define SIMPLY_CONCEPTS_HPP

#include <simply/type_traits.hpp>

namespace simply {

template <typename T>
concept mixin = simply::enable_mixin<T>;

template <typename T>
concept copy_mixin =
    simply::mixin<T> and simply::enable_mixin_tag<T, simply::copy_base>;

template <typename T>
concept move_mixin =
    simply::mixin<T> and simply::enable_mixin_tag<T, simply::move_base>;

template <typename T>
concept destroy_mixin =
    simply::mixin<T> and simply::enable_mixin_tag<T, simply::destroy_base>;

template <typename T>
concept storage =
    simply::mixin<T> and simply::enable_mixin_tag<T, simply::storage_base>;

template <typename T>
concept dispatch =
    simply::mixin<T> and simply::enable_mixin_tag<T, simply::dispatch_base>;

template <typename Mixin, typename T>
concept mixin_for = simply::mixin<Mixin> and simply::enable_mixin_for<Mixin, T>;

template <typename T, typename Mixin>
concept has_mixin = simply::mixin_for<Mixin, T>;

template <typename T, template <typename...> typename F>
concept specialization_of = simply::is_specialization_of_v<T, F>;

template <typename Derived, template <typename...> typename Base>
concept derived_from_specialization_of = requires(Derived *derived) {
  simply::_as_specialization_of<Base>(derived);
};

template <typename T>
concept compound =
    simply::mixin<T> and std::derived_from<T, simply::compound_base>;

template <typename T>
concept fundamental = simply::mixin<T> and not simply::compound<T>;

template <typename T>
concept composition =
    simply::compound<T> and
    simply::derived_from_specialization_of<T, simply::composes>;

template <typename T>
concept choice = simply::compound<T> and
                 simply::derived_from_specialization_of<T, simply::chooses>;

template <typename T>
concept member =
    simply::fundamental<T> and simply::enable_mixin_tag<T, simply::member_base>;

template <typename T>
concept constructor =
    simply::member<T> and simply::enable_mixin_tag<T, simply::constructor_base>;

template <typename T>
concept fundamental_copy_mixin =
    simply::constructor<T> and simply::enable_mixin_tag<T, simply::copy_base>;

template <typename T>
concept fundamental_move_mixin =
    simply::constructor<T> and simply::enable_mixin_tag<T, simply::move_base>;

template <typename T>
concept fundamental_destroy_mixin =
    simply::member<T> and simply::enable_mixin_tag<T, simply::destroy_base>;

template <typename T, typename U>
concept different_from = not std::same_as<T, U>;

template <simply::composition Composition, typename T>
inline constexpr bool enable_mixin_for<Composition, T> =
    simply::enable_mixin_for<simply::unique_fundamental_t<Composition>, T>;

template <simply::fundamental... Fundamental, typename T>
inline constexpr bool enable_mixin_for<simply::composes<Fundamental...>, T> =
    (... and simply::enable_mixin_for<Fundamental, T>);

template <simply::composition T, typename Tag>
inline constexpr bool enable_mixin_tag<T, Tag> =
    simply::enable_mixin_tag<simply::unique_fundamental_t<T>, Tag>;

template <simply::fundamental... Ts, typename Tag>
inline constexpr bool enable_mixin_tag<simply::composes<Ts...>, Tag> =
    (... or simply::enable_mixin_tag<Ts, Tag>);

template <simply::composition Composition, typename Tag>
struct fundamental_type<Composition, Tag>
    : simply::fundamental_type<simply::unique_fundamental_t<Composition>, Tag> {
};

template <simply::fundamental... Fundamental, typename Tag>
struct fundamental_type<simply::composes<Fundamental...>, Tag>
    : simply::fundamental_type<Fundamental, Tag>... {};

template <simply::fundamental Mixin, typename Tag>
  requires simply::enable_mixin_tag<Mixin, Tag>
struct fundamental_type<Mixin, Tag> : std::type_identity<Mixin> {};

// terminate on outer composes<>
template <typename Unique>
struct unique_fundamental<simply::composes<>, Unique>
    : std::type_identity<Unique> {};

// handle outer Fundamental as composes<Fundamental>
template <simply::fundamental Fundamental, typename Unique>
struct unique_fundamental<Fundamental, Unique>
    : simply::unique_fundamental<simply::composes<Fundamental>, Unique> {};

// handle outer Composition as composes<...>
template <simply::composition Composition, typename Unique>
struct unique_fundamental<Composition, Unique>
    : simply::unique_fundamental<simply::base_composition_t<Composition>,
                                 Unique> {};

// add inner Fundamental if different from all Unique
template <simply::fundamental Fundamental, typename... Rest,
          simply::different_from<Fundamental>... Unique>
struct unique_fundamental<simply::composes<Fundamental, Rest...>,
                          simply::composes<Unique...>>
    : simply::unique_fundamental<simply::composes<Rest...>,
                                 simply::composes<Unique..., Fundamental>> {};

// drop inner Fundamental otherwise
template <simply::fundamental Fundamental, typename... Rest, typename Unique>
struct unique_fundamental<simply::composes<Fundamental, Rest...>, Unique>
    : simply::unique_fundamental<simply::composes<Rest...>, Unique> {};

// handle inner Composition as composes<...>
template <simply::composition Composition, typename... Rest, typename Unique>
struct unique_fundamental<simply::composes<Composition, Rest...>, Unique>
    : simply::unique_fundamental<
          simply::composes<simply::base_composition_t<Composition>, Rest...>,
          Unique> {};

// handle inner composes<First...> as First...
template <typename... First, typename... Rest, typename Unique>
struct unique_fundamental<simply::composes<simply::composes<First...>, Rest...>,
                          Unique>
    : simply::unique_fundamental<simply::composes<First..., Rest...>, Unique> {
};

} // namespace simply

#endif // SIMPLY_CONCEPTS_HPP

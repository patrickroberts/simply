#ifndef SIMPLY_IMPL_HPP
#define SIMPLY_IMPL_HPP

#include <simply/concepts.hpp>

#include <utility>

namespace simply {

template <typename Affordance, typename T, typename Fn>
struct impl {
  static constexpr auto fn = Affordance::template fn<T>;
};

struct _deduce_tag_t {};

template <typename Affordance, typename T = simply::_deduce_tag_t>
inline constexpr const auto &fn = simply::impl<Affordance, T>::fn;

template <typename Affordance>
struct _deduce_t {
  template <typename T, typename... Args>
  static constexpr auto operator()(T &&self, Args &&...args) noexcept(
      noexcept(simply::fn<Affordance, std::remove_cvref_t<T>>(
          std::forward<T>(self), std::forward<Args>(args)...)))
      -> decltype(simply::fn<Affordance, std::remove_cvref_t<T>>(
          std::forward<T>(self), std::forward<Args>(args)...)) {
    return simply::fn<Affordance, std::remove_cvref_t<T>>(
        std::forward<T>(self), std::forward<Args>(args)...);
  }
};

template <typename Affordance>
inline constexpr simply::_deduce_t<Affordance>
    fn<Affordance, simply::_deduce_tag_t>{};

template <simply::member_affordance Affordance, typename T>
struct affordance_traits<Affordance, T> {
  using function_type = decltype(Affordance::template fn<T>);
};

// allows impl<impl<A, T>, Dyn>::fn to implement A for Dyn by type erasing T
template <typename Affordance, typename T, typename Dyn>
struct affordance_traits<simply::impl<Affordance, T>, Dyn>
    : affordance_traits<Affordance, Dyn> {};

} // namespace simply

#endif // SIMPLY_IMPL_HPP

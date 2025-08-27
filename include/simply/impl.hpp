#ifndef SIMPLY_IMPL_HPP
#define SIMPLY_IMPL_HPP

#include <simply/concepts.hpp>

#include <utility>

namespace simply {

template <typename Mixin, typename T, typename Fn>
struct impl {
  static constexpr auto fn = Mixin::template fn<T>;
};

struct _deduce_tag_t {};

template <typename Mixin, typename T = simply::_deduce_tag_t>
inline constexpr const auto &fn = simply::impl<Mixin, T>::fn;

template <typename Mixin>
struct _deduce_t {
  template <typename T, typename... Args>
  static constexpr auto operator()(T &&self, Args &&...args) noexcept(noexcept(
      simply::fn<Mixin, std::remove_cvref_t<T>>(std::forward<T>(self),
                                                std::forward<Args>(args)...)))
      -> decltype(simply::fn<Mixin, std::remove_cvref_t<T>>(
          std::forward<T>(self), std::forward<Args>(args)...)) {
    return simply::fn<Mixin, std::remove_cvref_t<T>>(
        std::forward<T>(self), std::forward<Args>(args)...);
  }
};

template <typename Mixin>
inline constexpr simply::_deduce_t<Mixin> fn<Mixin, simply::_deduce_tag_t>{};

template <simply::member Mixin, typename T>
struct mixin_traits<Mixin, T> {
  using function_type = decltype(Mixin::template fn<T>);
};

// allows impl<impl<A, T>, Dyn>::fn to implement A for Dyn by type erasing T
template <typename Mixin, typename T, typename Dyn>
struct mixin_traits<simply::impl<Mixin, T>, Dyn> : mixin_traits<Mixin, Dyn> {};

} // namespace simply

#endif // SIMPLY_IMPL_HPP

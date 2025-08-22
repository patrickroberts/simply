#ifndef SIMPLY_INVOCABLE_HPP
#define SIMPLY_INVOCABLE_HPP

#include <simply/iface.hpp>
#include <simply/impl.hpp>

namespace simply {

template <typename Fn, typename R, typename... Args>
concept invocable_with = requires {
  { std::declval<Fn>()(std::declval<Args>()...) } -> std::same_as<R>;
};

template <typename Fn, typename R, typename... Args>
concept nothrow_invocable_with = requires {
  { std::declval<Fn>()(std::declval<Args>()...) } noexcept -> std::same_as<R>;
};

template <typename Fn>
struct invocable {
  static_assert(std::is_function_v<Fn>);
};

template <typename R, typename... Args, bool NoExcept>
struct invocable<R(Args...) noexcept(NoExcept)>
    : simply::composes<simply::invocable<R(Args...) & noexcept(NoExcept)>,
                       simply::invocable<R(Args...) && noexcept(NoExcept)>> {};

template <typename R, typename... Args, bool NoExcept>
struct invocable<R(Args...) const noexcept(NoExcept)>
    : simply::composes<
          simply::invocable<R(Args...) const & noexcept(NoExcept)>,
          simply::invocable<R(Args...) const && noexcept(NoExcept)>> {};

template <typename R, typename... Args>
struct invocable<R(Args...) &> : simply::member_base {
  template <typename Fn>
    requires simply::invocable_with<Fn &, R, Args...>
  static constexpr auto fn(std::type_identity_t<Fn> &self, Args... args) -> R {
    return self(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) &&> : simply::member_base {
  template <typename Fn>
    requires simply::invocable_with<Fn, R, Args...>
  static constexpr auto fn(std::type_identity_t<Fn> &&self, Args... args) -> R {
    return std::move(self)(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) const &> : simply::member_base {
  template <typename Fn>
    requires simply::invocable_with<const Fn &, R, Args...>
  static constexpr auto fn(const std::type_identity_t<Fn> &self, Args... args)
      -> R {
    return self(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) const &&> : simply::member_base {
  template <typename Fn>
    requires simply::invocable_with<const Fn, R, Args...>
  static constexpr auto fn(const std::type_identity_t<Fn> &&self, Args... args)
      -> R {
    return std::move(self)(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) & noexcept> : simply::member_base {
  template <typename Fn>
    requires simply::nothrow_invocable_with<Fn &, R, Args...>
  static constexpr auto fn(std::type_identity_t<Fn> &self,
                           Args... args) noexcept -> R {
    return self(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) && noexcept> : simply::member_base {
  template <typename Fn>
    requires simply::nothrow_invocable_with<Fn, R, Args...>
  static constexpr auto fn(std::type_identity_t<Fn> &&self,
                           Args... args) noexcept -> R {
    return std::move(self)(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) const & noexcept> : simply::member_base {
  template <typename Fn>
    requires simply::nothrow_invocable_with<const Fn &, R, Args...>
  static constexpr auto fn(const std::type_identity_t<Fn> &self,
                           Args... args) noexcept -> R {
    return self(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args>
struct invocable<R(Args...) const && noexcept> : simply::member_base {
  template <typename Fn>
    requires simply::nothrow_invocable_with<const Fn, R, Args...>
  static constexpr auto fn(const std::type_identity_t<Fn> &&self,
                           Args... args) noexcept -> R {
    return std::move(self)(std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args, bool NoExcept, typename Self>
struct iface<simply::invocable<R(Args...) & noexcept(NoExcept)>, Self> {
  constexpr auto operator()(this Self &self, Args... args) noexcept(NoExcept)
      -> R {
    using invoke = simply::invocable<R(Args...) & noexcept(NoExcept)>;
    return simply::fn<invoke, Self>(self, std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args, bool NoExcept, typename Self>
struct iface<simply::invocable<R(Args...) && noexcept(NoExcept)>, Self> {
  constexpr auto operator()(this Self &&self, Args... args) noexcept(NoExcept)
      -> R {
    using invoke = simply::invocable<R(Args...) && noexcept(NoExcept)>;
    return simply::fn<invoke, Self>(std::move(self),
                                    std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args, bool NoExcept, typename Self>
struct iface<simply::invocable<R(Args...) const & noexcept(NoExcept)>, Self> {
  constexpr auto operator()(this const Self &self,
                            Args... args) noexcept(NoExcept) -> R {
    using invoke = simply::invocable<R(Args...) const & noexcept(NoExcept)>;
    return simply::fn<invoke, Self>(self, std::forward<Args>(args)...);
  }
};

template <typename R, typename... Args, bool NoExcept, typename Self>
struct iface<simply::invocable<R(Args...) const && noexcept(NoExcept)>, Self> {
  constexpr auto operator()(this const Self &&self,
                            Args... args) noexcept(NoExcept) -> R {
    using invoke = simply::invocable<R(Args...) const && noexcept(NoExcept)>;
    return simply::fn<invoke, Self>(std::move(self),
                                    std::forward<Args>(args)...);
  }
};

} // namespace simply

#endif // SIMPLY_INVOCABLE_HPP

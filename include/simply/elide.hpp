#ifndef SIMPLY_ELIDE_HPP
#define SIMPLY_ELIDE_HPP

#include <utility>

namespace simply {

template <typename F>
  requires std::is_reference_v<F>
class elide {
  using R = std::invoke_result_t<F>;
  static_assert(not std::is_reference_v<R>);

  F f;

public:
  template <typename G>
    requires std::constructible_from<F, G>
  constexpr elide(G &&g) noexcept(std::is_nothrow_constructible_v<F, G>)
      : f(std::forward<G>(g)) {}

  constexpr operator R() noexcept(std::is_nothrow_invocable_v<F>) {
    return std::forward<F>(f)();
  }

  constexpr auto operator()() noexcept(std::is_nothrow_invocable_v<F>) -> R {
    return std::forward<F>(f)();
  }

  elide() = delete;
  elide(const elide &) = delete;
  elide(elide &&) = delete;

  auto operator=(const elide &) -> elide & = delete;
  auto operator=(elide &&) -> elide & = delete;

  auto operator&() const volatile -> const volatile elide * = delete;

  template <typename U>
  void operator,(U &&) = delete;

  ~elide() = default;
};

template <typename F>
elide(F &&) -> elide<F &&>;

} // namespace simply

#endif // SIMPLY_ELIDE_HPP

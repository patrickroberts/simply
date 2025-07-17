#ifndef SIMPLY_SCOPE_HPP
#define SIMPLY_SCOPE_HPP

#include <utility>

namespace simply {

template <std::destructible EF>
  requires std::invocable<EF &>
class scope_exit {
  [[no_unique_address]] EF exit_function;
  bool active{true};

public:
  template <typename Fn>
    requires(not std::is_same_v<std::remove_cvref_t<Fn>, scope_exit>) and
            std::is_constructible_v<EF, Fn>
  constexpr explicit scope_exit(Fn &&fn) noexcept(
      std::is_nothrow_constructible_v<EF, Fn>)
      : exit_function(std::forward<Fn>(fn)) {}

  constexpr scope_exit(scope_exit &&other) noexcept(
      std::is_nothrow_move_constructible_v<EF> or
      std::is_nothrow_copy_constructible_v<EF>)
    requires std::is_nothrow_move_constructible_v<EF> or
                 std::is_copy_constructible_v<EF>
      : exit_function(std::move_if_noexcept(other).exit_function),
        active(other.active) {
    other.release();
  }

  scope_exit(const scope_exit &) = delete;

  auto operator=(const scope_exit &) -> scope_exit & = delete;
  auto operator=(scope_exit &&) -> scope_exit & = delete;

  constexpr void release() noexcept { active = false; }

  constexpr ~scope_exit() {
    if (active) {
      exit_function();
    }
  }
};

template <typename EF>
scope_exit(EF) -> scope_exit<EF>;

} // namespace simply

#endif // SIMPLY_SCOPE_HPP

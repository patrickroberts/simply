# Simply

## A playground for external polymorphism in C++

- No macros
- Concise syntax for defining and using mixins
- Supports allocators
- `constexpr` in C++26

```cpp
#include <simply/destructible.hpp>
#include <simply/dyn.hpp>

#include <iostream>
#include <numbers>
#include <vector>

struct insertable : simply::member_base {
  static auto fn(const auto &value, std::ostream &out) -> std::ostream & {
    return out << value;
  }
};

template <typename Self>
struct simply::iface<insertable, Self> {
  friend auto operator<<(std::ostream &out, const Self &self) -> std::ostream & {
    return simply::fn<insertable>(self, out);
  }
};

int main() {
  using namespace std::string_literals;

  struct mixins : simply::composes<insertable, simply::destructible> {};

  std::vector<simply::dyn<mixins>> values;
  values.emplace_back("Hello, world!"s);
  values.emplace_back(4);
  values.emplace_back(std::numbers::pi);

  for (const auto &value : values) {
    std::cout << value << '\n';
  }
}
```

### Supported Compilers

- Clang 19, 20
- GCC 14, 15

### Features

- `dyn<Mixin, Storage, Dispatch>` for type erasure of mixins
- Concepts:
  - `mixin<Mixin>`
  - `fundamental<Mixin>`
  - `compound<Mixin>`
  - `composition<Mixin>`
  - `choice<Mixin>`
  - `mixin_for<Mixin, T>`
  - `has_mixin<T, Mixin>`
- Predefined mixins for common use-cases:
  - `composes<Mixins...>`
  - `chooses<Mixins...>`
  - `destructible`
  - `movable`
  - `copyable`
  - `invocable<Fn>`
  - `extractable<In>`
  - `insertable<Out>`
  - `identifiable`
  - `allocator_storage<Allocator>`
  - `indirect_dispatch`
  - `inplace_dispatch`
- `iface<Mixin, Self>` to specialize interfaces of `dyn`
- `impl<Mixin, T>` to specialize implementations of a mixin

### Planned Features

- `inplace_storage<Size, Align>`
- `shared_storage`
- `copy_on_write_storage`
- `nullable`
- `retrievable`
- `trivially_copyable`
- `trivially_destructible`
- `equality_comparable<Policy>`
- `three_way_comparable<Ordering, Policy>`
  - Policy determines behavior when erased types mismatch
- `assignable_from<T>` support for type erasing sinks
- `slot_dispatch` for vtable lookup using a static slot map key
- Mixins for iterators
- Specializations of `impl` to delegate member mixins through:
  - `std::reference_wrapper`
  - `std::unique_ptr`
  - `std::shared_ptr`
  - `std::weak_ptr`
  - `std::indirect`
  - `std::polymorphic`
  - `std::variant` (if every alternative has the mixin)
  - `std::optional` (if both `T` and `std::nullopt_t` have the mixin)
  - `std::expected` (if both `T` and `E` have the mixin)
  - `std::pair` (if either `first` or `second` exclusively has the mixin)
  - `std::tuple` (if one element exclusively has the mixin)
- Specializations of `dyn` that behave equivalently to:
  - `std::function`
  - `std::copyable_function`
  - `std::move_only_function`
  - `std::function_ref`
  - `std::any`
  - `std::ranges::any_view`

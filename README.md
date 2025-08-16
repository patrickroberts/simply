# Simply

## A playground for external polymorphism in C++.

- No macros
- Concise syntax for defining and using affordances
- Supports allocators
- `constexpr` in C++26

```cpp
#include <simply/destructible.hpp>
#include <simply/dyn.hpp>

#include <iostream>
#include <numbers>
#include <vector>

struct insertable : simply::member_affordance_base {
  static auto fn(const auto &value, std::ostream &out) -> std::ostream & {
    return out << value;
  }
};

template <typename Self>
struct simply::iface<insertable, Self> {
  friend auto operator<<(std::ostream &out, const Self &self) -> std::ostream & {
    return simply::fn<insertable, Self>(self, out);
  }
};

int main() {
  using namespace std::string_literals;

  struct affordances : simply::composes<insertable, simply::destructible> {};

  std::vector<simply::dyn<affordances>> values;
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

- `dyn<Affordance, Storage, Dispatch>` for type erasure of affordances
- Concepts:
  - `affordance<Affordance>`
  - `fundamental_affordance<Affordance>`
  - `compound_affordance<Affordance>`
  - `composition_affordance<Affordance>`
  - `choice_affordance<Affordance>`
  - `affords<T, Affordance>`
- Predefined affordances for common use-cases:
  - `composes<Affordances...>`
  - `chooses<Affordances...>`
  - `destructible`
  - `move_constructible`
  - `copy_constructible`
  - `extractable<In>`
  - `insertable<Out>`
  - `allocator_storage<Allocator>`
  - `indirect_dispatch`
  - `inplace_dispatch`
- `iface<Affordance, Self>` to specialize interfaces of `dyn`
- `impl<Affordance, T>` to specialize implementations of an affordance

### Planned Features

- `inplace_storage<Size, Align>`
- `shared_storage`
- `copy_on_write_storage`
- `invocable` affordance template
- `assignment_affordance` support for type erasing sinks
- `slot_dispatch` for vtable lookup using a static slot map key
- Affordances for iterators
- Specializations of `impl` to delegate member affordances through:
  - `std::reference_wrapper`
  - `std::unique_ptr`
  - `std::shared_ptr`
  - `std::weak_ptr`
  - `std::indirect`
  - `std::polymorphic`
  - `std::variant` (if every alternative affords the member)
  - `std::optional` (if both `T` and `std::nullopt_t` afford the member)
  - `std::expected` (if both `T` and `E` afford the member)
  - `std::pair` (if either first or second exclusively affords the member)
  - `std::tuple` (if one element exclusively affords the member)
- Specializations of `dyn` that behave equivalently to:
  - `std::function`
  - `std::copyable_function`
  - `std::move_only_function`
  - `std::function_ref`
  - `std::any`
  - `std::ranges::any_view`

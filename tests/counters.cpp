#include <simply/copyable.hpp>
#include <simply/dyn.hpp>

#include <algorithm>
#include <cassert>
#include <vector>

// example of a class with a label that counts how many times it is copied
template <char Label>
struct counter {
  constexpr counter() = default;
  constexpr counter(const counter &other) : copied(other.copied + 1) {}
  counter(counter &&other) = delete;

  auto operator=(const counter &) -> counter & = delete;
  auto operator=(counter &&) -> counter & = delete;

  constexpr ~counter() = default;

  [[nodiscard]] constexpr auto copy_count() const { return copied; }
  [[nodiscard]] constexpr auto label() const { return Label; }

private:
  int copied = 0;
};

// example of an affordance for a class that counts how many times it is copied
template <typename CountT>
struct copy_countable : simply::member_affordance_base {
  template <typename T>
  static constexpr auto fn(const T &counter) -> CountT {
    return counter.copy_count();
  }
};

// example of an affordance for a class that is labeled
template <typename LabelT>
struct labeled : simply::member_affordance_base {
  template <typename T>
  static constexpr auto fn(const T &counter) -> LabelT {
    return counter.label();
  }
};

// specialize iface for each affordance so dyn<Affordance> can use them
template <typename CountT, typename Self>
struct simply::iface<copy_countable<CountT>, Self> {
  constexpr auto copy_count(this const Self &self) -> CountT {
    return simply::fn<copy_countable<CountT>>(self);
  }
};

template <typename LabelT, typename Self>
struct simply::iface<labeled<LabelT>, Self> {
  constexpr auto label(this const Self &self) -> LabelT {
    return simply::fn<labeled<LabelT>>(self);
  }
};

// test dyn<countable> at compile-time
static_assert([] {
  struct countable
      : simply::composes<copy_countable<int>, labeled<char>,
                         simply::copy_constructible, simply::destructible> {};

  // initialize a vector of dyn<countable> with three different counter types
  std::vector<simply::dyn<countable>> v;
  v.emplace_back(std::in_place_type<counter<'A'>>);
  v.emplace_back(std::in_place_type<counter<'B'>>);
  v.emplace_back(std::in_place_type<counter<'C'>>);

  assert(v[0].copy_count() == 0 and v[0].label() == 'A' and
         v[1].copy_count() == 0 and v[1].label() == 'B' and
         v[2].copy_count() == 0 and v[2].label() == 'C');

  std::ranges::reverse(v);

  // elements are reversed by swapping them, so copy_count remains 0
  assert(v[0].copy_count() == 0 and v[0].label() == 'C' and
         v[1].copy_count() == 0 and v[1].label() == 'B' and
         v[2].copy_count() == 0 and v[2].label() == 'A');

  // copy the vector...
  auto w = v;

  // ...which copies each element, so copy_count is incremented
  assert(w[0].copy_count() == 1 and w[0].label() == 'C' and
         w[1].copy_count() == 1 and w[1].label() == 'B' and
         w[2].copy_count() == 1 and w[2].label() == 'A');

  // success
  return true;
}());

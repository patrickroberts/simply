#include <simply/copyable.hpp>
#include <simply/destructible.hpp>
#include <simply/dyn.hpp>
#include <simply/movable.hpp>

struct semiregular {
  semiregular();
  semiregular(const semiregular &);
  semiregular(semiregular &&) noexcept;

  auto operator=(const semiregular &) -> semiregular &;
  auto operator=(semiregular &&) noexcept -> semiregular &;

  ~semiregular();
};

struct uncopyable {
  uncopyable();
  uncopyable(const uncopyable &) = delete;
  uncopyable(uncopyable &&) noexcept;

  auto operator=(const uncopyable &) -> uncopyable &;
  auto operator=(uncopyable &&) noexcept -> uncopyable &;

  ~uncopyable();
};

struct immovable {
  immovable();
  immovable(const immovable &);
  immovable(immovable &&) = delete;

  auto operator=(const immovable &) -> immovable &;
  auto operator=(immovable &&) noexcept -> immovable &;

  ~immovable();
};

struct indestructible {
  indestructible();
  indestructible(const indestructible &);
  indestructible(indestructible &&) noexcept;

  auto operator=(const indestructible &) -> indestructible &;
  auto operator=(indestructible &&) noexcept -> indestructible &;

  ~indestructible() = delete;
};

using unusable = simply::dyn<simply::composes<>>;
using copyable = simply::dyn<simply::copyable>;
using movable = simply::dyn<simply::movable>;
using destructible = simply::dyn<simply::destructible>;

template <typename T, typename U>
concept constructible_from_in_place =
    simply::constructible_from<T, std::in_place_type_t<U>>;

static_assert(constructible_from_in_place<unusable, semiregular>);
static_assert(constructible_from_in_place<unusable, uncopyable>);
static_assert(constructible_from_in_place<unusable, immovable>);
static_assert(constructible_from_in_place<unusable, indestructible>);

static_assert(not simply::copy_constructible<unusable>);
// move constructible because allocator_storage
static_assert(simply::move_constructible<unusable>);
static_assert(not std::is_destructible_v<unusable>);

static_assert(constructible_from_in_place<copyable, semiregular>);
static_assert(not constructible_from_in_place<copyable, uncopyable>);
static_assert(constructible_from_in_place<copyable, immovable>);
// TODO reimplement copyable not to depend on destruction
// clang is the only compiler that thinks it doesn't
#if defined(__clang__)
static_assert(constructible_from_in_place<copyable, indestructible>);
#endif

static_assert(simply::copy_constructible<copyable>);
// move constructible because allocator_storage
static_assert(simply::move_constructible<copyable>);
static_assert(not std::is_destructible_v<copyable>);

static_assert(constructible_from_in_place<movable, semiregular>);
static_assert(constructible_from_in_place<movable, uncopyable>);
static_assert(not constructible_from_in_place<movable, immovable>);
// TODO reimplement movable not to depend on destruction
// clang is the only compiler that thinks it doesn't
#if defined(__clang__)
static_assert(constructible_from_in_place<movable, indestructible>);
#endif

static_assert(not simply::copy_constructible<movable>);
static_assert(simply::move_constructible<movable>);
static_assert(not std::is_destructible_v<movable>);

static_assert(constructible_from_in_place<destructible, semiregular>);
static_assert(constructible_from_in_place<destructible, uncopyable>);
static_assert(constructible_from_in_place<destructible, immovable>);
static_assert(not constructible_from_in_place<destructible, indestructible>);

static_assert(not simply::copy_constructible<destructible>);
// move constructible because allocator_storage
static_assert(simply::move_constructible<destructible>);
static_assert(std::is_destructible_v<destructible>);

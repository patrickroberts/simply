#ifndef SIMPLY_STORAGE_HPP
#define SIMPLY_STORAGE_HPP

#include <simply/copyable.hpp>
#include <simply/elide.hpp>
#include <simply/iface.hpp>
#include <simply/impl.hpp>
#include <simply/scope.hpp>

#include <memory>

namespace simply {

template <typename T, typename Alloc>
[[nodiscard]] constexpr auto _rebind_alloc(Alloc alloc) {
  using allocator_type = std::allocator_traits<Alloc>::template rebind_alloc<T>;
  return allocator_type(alloc);
}

template <typename Rebound, typename... Args>
[[nodiscard]] static constexpr auto _construct_with(Rebound rebound,
                                                    Args &&...args) {
  using rebound_traits = std::allocator_traits<Rebound>;
  auto pointer = rebound_traits::allocate(rebound, 1);
  // defer deallocation if construct throws
  auto dealloc = simply::scope_exit{
      [&] { rebound_traits::deallocate(rebound, pointer, 1); },
  };

  rebound_traits::construct(rebound, std::to_address(pointer),
                            std::forward<Args>(args)...);
  // cancel deallocation if construct returns
  dealloc.release();
  return pointer;
}

template <typename Allocator = std::allocator<std::byte>>
struct allocator_storage : simply::storage_affordance_base {
  using traits = std::allocator_traits<Allocator>;
  using void_pointer = traits::void_pointer;
  using const_void_pointer = traits::const_void_pointer;

  template <typename T>
  struct dispatch : simply::member_affordance_base {
    using rebound = traits::template rebind_traits<T>;
    using pointer = rebound::pointer;
    using const_pointer = rebound::const_pointer;

    template <typename Self>
    struct fn_t {
      static constexpr auto operator()(Self &self) noexcept {
        return static_cast<pointer>(self.get());
      }

      static constexpr auto operator()(const Self &self) noexcept {
        return static_cast<const_pointer>(self.get());
      }
    };

    template <typename Self>
    static constexpr fn_t<Self> fn{};
  };
};

// TODO resolve storage type from choices by checking each compatibility with T
template <typename T, typename Dyn>
inline constexpr const auto &_dispatch_fn =
    simply::fn<typename Dyn::storage_type::template dispatch<T>>;

template <simply::fundamental_copy_affordance Copy, typename T,
          simply::specialization_of<simply::allocator_storage> Storage,
          typename Dyn, typename R, typename Self, typename... Args,
          bool NoExcept>
struct impl<simply::impl<Copy, T>, simply::iface<Storage, Dyn>,
            R(Self, Args...) noexcept(NoExcept)> {
  using iface_type = simply::iface<Storage, Dyn>;
  static_assert(std::same_as<R(Self, Args...) noexcept(NoExcept),
                             iface_type(const iface_type &)>);

  // static dispatch for copy affordance of dyn with allocator storage
  static constexpr auto fn(Self self) -> R {
    using type = typename simply::iface<Storage, Dyn>::allocator_type;
    using traits = std::allocator_traits<type>;

    const auto alloc =
        traits::select_on_container_copy_construction(self.get_allocator());

    if (self.valueless_after_move()) {
      return R{std::allocator_arg, alloc, impl{}};
    }

    return R{
        std::allocator_arg,
        alloc,
        std::in_place_type<T>,
        simply::elide([&] -> T {
          const auto pointer = simply::_dispatch_fn<T, Dyn>(self);
          return simply::fn<Copy, T>(*pointer);
        }),
    };
  }

private:
  impl() noexcept = default;
};

template <simply::specialization_of<simply::allocator_storage> Storage,
          typename Self>
  requires simply::affordance<Storage>
struct iface<Storage, Self> {
private:
  using traits = Storage::traits;

  [[nodiscard]] constexpr auto _release() noexcept {
    return std::exchange(object_ptr, nullptr);
  }

public:
  using allocator_type = traits::allocator_type;
  using void_pointer = traits::void_pointer;
  using const_void_pointer = traits::const_void_pointer;

  // effectively private "valueless" constructor
  template <typename Alloc, simply::fundamental_copy_affordance Copy,
            typename T>
  constexpr iface(std::allocator_arg_t alloc_tag, const Alloc &alloc,
                  simply::impl<simply::impl<Copy, T>, iface> tag) noexcept
    requires std::default_initializable<void_pointer>
      : alloc(alloc), object_ptr() {}

  constexpr iface(const iface &other) {
    using copy = simply::copy_affordance_t<typename Self::affordance_type>;

    std::construct_at(this, simply::elide([&] {
                        // TODO template storage on dispatch to access
                        // get_member() without downcasting
                        return simply::fn<copy>(
                            static_cast<const Self &>(other));
                      }));
  }

  constexpr iface(iface &&other) noexcept
      : alloc(other.alloc), object_ptr(other._release()) {}

  template <typename Alloc, typename T, typename... Args>
  constexpr explicit iface([[maybe_unused]] std::allocator_arg_t alloc_tag,
                           const Alloc &alloc,
                           [[maybe_unused]] std::in_place_type_t<T> obj_tag,
                           Args &&...args)
      : alloc(alloc), object_ptr(simply::_construct_with(
                          simply::_rebind_alloc<T>(this->alloc),
                          std::forward<Args>(args)...)) {}

  template <typename T, typename... Args>
  constexpr explicit iface([[maybe_unused]] std::in_place_type_t<T> tag,
                           Args &&...args)
    requires std::default_initializable<allocator_type>
      : alloc(),
        object_ptr(simply::_construct_with(simply::_rebind_alloc<T>(alloc),
                                           std::forward<Args>(args)...)) {}

  auto operator=(const iface &other) -> iface & = default;
  auto operator=(iface &&other) noexcept -> iface & = default;

  ~iface() = default;

  [[nodiscard]] constexpr auto get_allocator() const noexcept
      -> allocator_type {
    return alloc;
  }

  [[nodiscard]] constexpr auto get() noexcept -> void_pointer {
    return object_ptr;
  }

  [[nodiscard]] constexpr auto get() const noexcept -> const_void_pointer {
    return object_ptr;
  }

  [[nodiscard]] constexpr auto valueless_after_move() const noexcept -> bool {
    return object_ptr == nullptr;
  }

private:
  [[no_unique_address]] allocator_type alloc;
  void_pointer object_ptr;
};

} // namespace simply

#endif // SIMPLY_STORAGE_HPP

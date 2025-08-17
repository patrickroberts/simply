#ifndef SIMPLY_STORAGE_HPP
#define SIMPLY_STORAGE_HPP

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

template <typename Allocator = std::allocator<std::byte>>
struct allocator_storage : simply::storage_affordance_base {
  using traits = std::allocator_traits<Allocator>;
  using void_pointer = traits::void_pointer;
  using const_void_pointer = traits::const_void_pointer;

  template <typename T>
  struct fn_t {
    using rebound = traits::template rebind_traits<T>;
    using pointer = rebound::pointer;
    using const_pointer = rebound::const_pointer;

    static constexpr auto operator()(void_pointer ptr) noexcept {
      return static_cast<pointer>(ptr);
    }

    static constexpr auto operator()(const_void_pointer ptr) noexcept {
      return static_cast<const_pointer>(ptr);
    }
  };

  template <typename T>
  static constexpr fn_t<T> fn{};
};

template <simply::specialization_of<simply::allocator_storage> Storage,
          typename Self>
  requires simply::affordance<Storage>
struct iface<Storage, Self> {
private:
  using traits = Storage::traits;

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

  [[nodiscard]] constexpr auto _release() noexcept {
    return std::exchange(object_ptr, nullptr);
  }

public:
  using allocator_type = traits::allocator_type;
  using void_pointer = traits::void_pointer;
  using const_void_pointer = traits::const_void_pointer;

  // BUG use this->alloc instead of other.alloc to copy other->object_ptr
  constexpr iface(const iface &other)
      : alloc(traits::select_on_container_copy_construction(other.alloc)),
        object_ptr(
            other.valueless_after_move()
                ? nullptr
                : simply::fn<
                      simply::copy_affordance_t<typename Self::affordance_type>,
                      Self>(static_cast<const Self &>(other))
                      ._release()) {}

  constexpr iface(iface &&other) noexcept
      : alloc(other.alloc), object_ptr(other._release()) {}

  template <typename Alloc, typename T, typename... Args>
  constexpr explicit iface([[maybe_unused]] std::allocator_arg_t alloc_tag,
                           const Alloc &alloc,
                           [[maybe_unused]] std::in_place_type_t<T> obj_tag,
                           Args &&...args)
      : alloc(alloc),
        object_ptr(_construct_with(simply::_rebind_alloc<T>(this->alloc),
                                   std::forward<Args>(args)...)) {}

  template <typename T, typename... Args>
  constexpr explicit iface([[maybe_unused]] std::in_place_type_t<T> tag,
                           Args &&...args)
    requires std::default_initializable<allocator_type>
      : alloc(), object_ptr(_construct_with(simply::_rebind_alloc<T>(alloc),
                                            std::forward<Args>(args)...)) {}

  constexpr auto operator=(const iface &other) -> iface & = delete;
  constexpr auto operator=(iface &&other) noexcept -> iface & = delete;

  constexpr ~iface() noexcept = default;

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

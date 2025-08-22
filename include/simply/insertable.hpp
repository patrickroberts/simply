#ifndef SIMPLY_INSERTABLE_HPP
#define SIMPLY_INSERTABLE_HPP

#include <simply/iface.hpp>
#include <simply/impl.hpp>

namespace simply {

template <typename T, typename Out>
concept insertable_into = requires(const T &value, Out &output) {
  { output << value } -> std::same_as<Out &>;
};

template <typename Out>
struct insertable : simply::member_base {
  static constexpr auto fn(const simply::insertable_into<Out> auto &value,
                           Out &out) -> Out & {
    return out << value;
  }
};

template <typename Out, typename Self>
struct iface<simply::insertable<Out>, Self> {
  friend constexpr auto operator<<(Out &out, const Self &self) -> Out & {
    return simply::fn<simply::insertable<Out>, Self>(self, out);
  }
};

} // namespace simply

#endif // SIMPLY_INSERTABLE_HPP

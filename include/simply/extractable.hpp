#ifndef SIMPLY_EXTRACTABLE_HPP
#define SIMPLY_EXTRACTABLE_HPP

#include <simply/iface.hpp>
#include <simply/impl.hpp>

namespace simply {

template <typename T, typename In>
concept extractable_from = requires(const T &value, In &input) {
  { input >> value } -> std::same_as<In &>;
};

template <typename In>
struct extractable : simply::member_affordance_base {
  static constexpr auto fn(const simply::extractable_from<In> auto &value,
                           In &out) -> In & {
    return out >> value;
  }
};

template <typename In, typename Self>
struct iface<simply::extractable<In>, Self> {
  friend constexpr auto operator>>(In &out, const Self &self) -> In & {
    return simply::impl<simply::extractable<In>, Self>::fn(self, out);
  }
};

} // namespace simply

#endif // SIMPLY_EXTRACTABLE_HPP

#ifndef SLOT_SLOT_HPP
#define SLOT_SLOT_HPP

#include <type_traits>
#include <utility>

namespace slot {

template<class Policy, class T>
class binding {
public:
  using policy_type = Policy;

  template<class U>
  constexpr binding(U&& val, Policy policy) noexcept(std::conjunction_v<std::is_nothrow_constructible<T, U>, std::is_nothrow_move_constructible<Policy>>)
      : value_(std::forward<U>(val)), policy_(std::move(policy))
  {
  }

  binding(binding const&)
    requires std::is_copy_constructible_v<T>
  = default;
  binding(binding&&)
    requires std::is_move_constructible_v<T>
  = default;

  binding(binding const&) = delete;
  binding(binding&&) = delete;

  binding& operator=(binding const&) = delete;
  binding& operator=(binding&&) = delete;

  template<class Self>
  constexpr decltype(auto) get(this Self&& self) noexcept
  {
    if constexpr (std::is_lvalue_reference_v<T>) {
      return self.value_;
    } else {
      return std::forward_like<Self>(self.value_);
    }
  }

  template<class Self>
  constexpr decltype(auto) policy(this Self&& self) noexcept
  {
    return std::forward_like<Self>(self.policy_);
  }

private:
  [[no_unique_address]] Policy policy_;
  T value_;
};

template<class Policy, class T>
[[nodiscard]] constexpr binding<Policy, T> into(T&& val, Policy policy = Policy{}) noexcept(std::is_nothrow_move_constructible_v<Policy>)
{
  return binding<Policy, T>(std::forward<T>(val), std::move(policy));
}

template<class T>
struct is_binding : std::false_type {};

template<class Policy, class T>
struct is_binding<binding<Policy, T>> : std::true_type {};

template<class T>
inline constexpr bool is_binding_v = is_binding<T>::value;

template<class Destination, class Source>
concept writable = requires(Destination&& d, Source&& s) {
  static_cast<Destination&&>(d) = static_cast<Source&&>(s);
  static_cast<Destination const&&>(d) = static_cast<Source&&>(s);
};

}  // namespace slot

#endif  // SLOT_SLOT_HPP

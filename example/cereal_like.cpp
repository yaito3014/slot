#include <slot/slot.hpp>

#include <iostream>
#include <string_view>
#include <type_traits>
#include <utility>

namespace demo {

struct nvp_policy {
  std::string_view name;
};

constexpr nvp_policy nvp(std::string_view name) noexcept { return {name}; }

class json_archive {
public:
  explicit json_archive(std::ostream& out) : out_(out) {}

  template<class... Args>
  json_archive& operator()(Args&&... args)
  {
    (process(std::forward<Args>(args)), ...);
    return *this;
  }

private:
  template<class T>
  void process(T&& arg)
  {
    if constexpr (slot::is_binding_v<std::remove_cvref_t<T>>) {
      auto policy = std::forward<T>(arg).policy();
      write(policy.name, std::forward<T>(arg).get());
    } else {
      write("<anon>", std::forward<T>(arg));
    }
  }

  template<class V>
  void write(std::string_view name, V const& value)
  {
    out_ << "  \"" << name << "\": " << value << ",\n";
  }

  std::ostream& out_;
};

}  // namespace demo

struct point {
  int x;
  int y;
};

template<class Ar>
void serialize(Ar& ar, point const& p)
{
  ar(slot::into(p.x, demo::nvp("x")), slot::into(p.y, demo::nvp("y")));
}

int main()
{
  demo::json_archive ar(std::cout);
  point p{3, 4};

  std::cout << "{\n";
  serialize(ar, p);
  ar(99);
  std::cout << "}\n";
}

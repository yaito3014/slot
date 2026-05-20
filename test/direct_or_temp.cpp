#include <slot/slot.hpp>

#include <catch2/catch_test_macros.hpp>

#include <utility>

namespace {

struct direct_assign {
  template<class Dest, class Source>
  constexpr void write(Dest&& dest, Source&& source) const
  {
    std::forward<Dest>(dest) = std::forward<Source>(source);
  }
};

template<class TemporaryType>
struct assign_temporary {
  template<class Dest, class Source>
  constexpr void write(Dest&& dest, Source&& source) const
  {
    TemporaryType temporary(std::forward<Source>(source));
    std::forward<Dest>(dest) = std::move(temporary);
  }
};

class special_int {
public:
  explicit special_int(int value) noexcept : value_(value) {}

  special_int& operator=(int value) noexcept
  {
    value_ = value;
    return *this;
  }

  int value() const noexcept { return value_; }

private:
  int value_;
};

struct int_proxy {
  int* target;

  template<class Self>
  Self&& operator=(this Self&& self, int value) noexcept
  {
    *self.target = value;
    return std::forward<Self>(self);
  }
};

class sequential_writer {
public:
  explicit sequential_writer(int start) noexcept : next_(start) {}

  template<class... Ts>
  void operator()(Ts&&... xs)
  {
    (process(xs), ...);
  }

private:
  template<class T>
  void process(T&& x)
  {
    if constexpr (slot::is_binding_v<std::remove_cvref_t<T>>) {
      auto&& policy = std::forward<T>(x).policy();
      std::forward<decltype(policy)>(policy).write(std::forward<T>(x).get(), next_++);
    } else {
      std::forward<T>(x) = next_++;
    }
  }

  int next_;
};

}  // namespace

TEST_CASE("direct_assign writes through to lvalue")
{
  int x = 0;
  sequential_writer{100}(slot::into<direct_assign>(x));
  CHECK(x == 100);
}

TEST_CASE("assign_temporary constructs intermediate then assigns")
{
  special_int s(0);
  sequential_writer{42}(slot::into<assign_temporary<int>>(s));
  CHECK(s.value() == 42);
}

TEST_CASE("bare lvalue is written directly")
{
  int x = 0;
  sequential_writer{7}(x);
  CHECK(x == 7);
}

TEST_CASE("mixed pack writes in declared order")
{
  int a = 0;
  int c = 0;
  special_int b(0);
  sequential_writer{10}(a, slot::into<direct_assign>(c), slot::into<assign_temporary<int>>(b));
  CHECK(a == 10);
  CHECK(c == 11);
  CHECK(b.value() == 12);
}

TEST_CASE("proxy reference receives writes through binding")
{
  int storage = 0;
  sequential_writer{99}(slot::into<direct_assign>(int_proxy{&storage}));
  CHECK(storage == 99);
}

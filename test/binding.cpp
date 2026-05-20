#include <slot/slot.hpp>

#include <catch2/catch_test_macros.hpp>

#include <type_traits>
#include <utility>

namespace {

struct empty_policy {};

struct stateful_policy {
  int n;
};

struct move_only {
  move_only() = default;
  move_only(move_only const&) = delete;
  move_only(move_only&&) = default;
};

}  // namespace

TEST_CASE("is_binding_v identifies bindings")
{
  using slot::binding;
  using slot::is_binding_v;

  STATIC_CHECK(is_binding_v<binding<empty_policy, int&>>);
  STATIC_CHECK(is_binding_v<binding<empty_policy, int>>);
  STATIC_CHECK(!is_binding_v<int>);
  STATIC_CHECK(!is_binding_v<empty_policy>);
}

TEST_CASE("into deduces reference for lvalue, value for rvalue")
{
  using slot::binding;

  int x = 0;
  int const cx = 0;

  STATIC_CHECK(std::is_same_v<decltype(slot::into<empty_policy>(x)), binding<empty_policy, int&>>);
  STATIC_CHECK(std::is_same_v<decltype(slot::into<empty_policy>(cx)), binding<empty_policy, int const&>>);
  STATIC_CHECK(std::is_same_v<decltype(slot::into<empty_policy>(42)), binding<empty_policy, int>>);
  STATIC_CHECK(std::is_same_v<decltype(slot::into<empty_policy>(std::move(x))), binding<empty_policy, int>>);
}

TEST_CASE("binding copy/move-constructibility tracks T")
{
  using slot::binding;

  STATIC_CHECK(std::is_copy_constructible_v<binding<empty_policy, int&>>);
  STATIC_CHECK(std::is_move_constructible_v<binding<empty_policy, int&>>);

  STATIC_CHECK(std::is_copy_constructible_v<binding<empty_policy, int>>);
  STATIC_CHECK(std::is_move_constructible_v<binding<empty_policy, int>>);

  STATIC_CHECK(!std::is_copy_constructible_v<binding<empty_policy, move_only>>);
  STATIC_CHECK(std::is_move_constructible_v<binding<empty_policy, move_only>>);
}

TEST_CASE("binding assignment is always disabled")
{
  using slot::binding;

  STATIC_CHECK(!std::is_copy_assignable_v<binding<empty_policy, int&>>);
  STATIC_CHECK(!std::is_move_assignable_v<binding<empty_policy, int&>>);
  STATIC_CHECK(!std::is_copy_assignable_v<binding<empty_policy, int>>);
  STATIC_CHECK(!std::is_move_assignable_v<binding<empty_policy, int>>);
}

TEST_CASE("get() returns T& for ref-mode, tracks Self category for owned-mode")
{
  using slot::binding;

  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int&>&>().get()), int&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int&>&&>().get()), int&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int&> const&>().get()), int&>);

  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int>&>().get()), int&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int>&&>().get()), int&&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<empty_policy, int> const&>().get()), int const&>);
}

TEST_CASE("policy() return category tracks Self regardless of T")
{
  using slot::binding;

  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<stateful_policy, int&>&>().policy()), stateful_policy&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<stateful_policy, int&>&&>().policy()), stateful_policy&&>);
  STATIC_CHECK(std::is_same_v<decltype(std::declval<binding<stateful_policy, int&> const&>().policy()), stateful_policy const&>);
}

TEST_CASE("[[no_unique_address]] collapses empty policy")
{
  STATIC_CHECK(sizeof(slot::binding<empty_policy, int>) == sizeof(int));
  STATIC_CHECK(sizeof(slot::binding<empty_policy, double>) == sizeof(double));
}

TEST_CASE("get() on ref-mode binding writes through to lvalue target")
{
  int x = 5;
  auto b = slot::into<empty_policy>(x);
  b.get() = 42;
  CHECK(x == 42);
}

TEST_CASE("get() on owned binding allows reading and writing the owned value")
{
  auto b = slot::into<empty_policy>(7);
  CHECK(b.get() == 7);
  b.get() = 99;
  CHECK(b.get() == 99);
}

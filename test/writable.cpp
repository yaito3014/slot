#include <slot/slot.hpp>

#include <catch2/catch_test_macros.hpp>

#include <tuple>

TEST_CASE("writable")
{
  using slot::writable;

  // plain type cannot be written
  STATIC_CHECK(!writable<int, int>);
  STATIC_CHECK(!writable<int, int&>);
  STATIC_CHECK(!writable<int, int&&>);

  // reference type can be written
  STATIC_CHECK(writable<int&, int>);
  STATIC_CHECK(writable<int&, int&>);
  STATIC_CHECK(writable<int&, int&&>);

  // proxy reference can be written
  STATIC_CHECK(writable<std::tuple<int&>, std::tuple<int>>);
  STATIC_CHECK(writable<std::tuple<int&>, std::tuple<int>&>);
  STATIC_CHECK(writable<std::tuple<int&>, std::tuple<int>&&>);
}

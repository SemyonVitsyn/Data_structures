#include <catch.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "test_util.hpp"
#include "vector.hpp"



TEST_CASE("std::move_if_noexcept") {
  Vector<MoveThrowable> v{MoveThrowable{}};
  Vector<MoveThrowable> move_constructed(std::move(v));
  REQUIRE_FALSE(MoveThrowable::was_moved);
  v = std::move(move_constructed);
  REQUIRE_FALSE(MoveThrowable::was_moved);
}

TEST_CASE("Size Constructor", "[Safety]") {
  Throwable::until_throw = 5;
  REQUIRE_THROWS_AS(Vector<Throwable>(10u), Exception);
}

TEST_CASE("Value Constructor", "[Safety]") {
  Throwable::until_throw = 5;
  REQUIRE_THROWS_AS(Vector<Throwable>(10u, Throwable{}), Exception);

  try {
    Throwable::until_throw = 15;
    Vector<Throwable> v(10u, Throwable{});
  } catch (Exception&) {
  }
}

TEST_CASE("Iterators Constructor", "[Safety]") {
  Throwable::until_throw = 210;
  const std::vector<Throwable> values(100u, Throwable{});
  Throwable::until_throw = 50;
  REQUIRE_THROWS_AS(Vector<Throwable>(values.begin(), values.end()), Exception);

  try {
    Throwable::until_throw = 150;
    Vector<Throwable> v(values.begin(), values.end());
  } catch (Exception&) {
  }
}

TEST_CASE("Initializer List Constructor", "[Safety]") {
  Throwable::until_throw = 6;
  REQUIRE_THROWS_AS(Vector<Throwable>({Throwable{}, Throwable{}, Throwable{}, Throwable{}}), Exception);

  try {
    Throwable::until_throw = 10;
    Vector<Throwable> v({Throwable{}, Throwable{}, Throwable{}, Throwable{}});
  } catch (Exception&) {
  }
}

TEST_CASE("Copy Constructor Safety", "[Safety]") {
  Throwable::until_throw = 210;
  const Vector<Throwable> values(100u, Throwable{});
  Throwable::until_throw = 50;
  REQUIRE_THROWS_AS(Vector<Throwable>(values), Exception);

  try {
    Throwable::until_throw = 150;
    Vector<Throwable> v(values);
  } catch (Exception&) {
  }
}

TEST_CASE("Move Constructor Safety", "[Safety]") {
  Throwable::until_throw = 210;
  Vector<Throwable> values(100u, Throwable{});
  Throwable::until_throw = 1;
  REQUIRE_NOTHROW(Vector<Throwable>(std::move(values)));
}

TEST_CASE("Copy Assignment Safety", "[Safety]") {
  {
    Throwable::until_throw = 12;
    const Vector<Throwable> values(5u);

    Vector<Throwable> v;
    Throwable::until_throw = 3;
    REQUIRE_THROWS_AS(v = values, Exception);
    REQUIRE(v.Capacity() >= v.Size());

    try {
      Vector<Throwable> vv;
      Throwable::until_throw = 8;
      vv = values;
    } catch (Exception&) {
    }
  }

  {
    Throwable::until_throw = 100;
    const Vector<Throwable> values(10u);
    Vector<Throwable> v(35u);
    Throwable::until_throw = 5;
    REQUIRE_THROWS_AS(v = values, Exception);
    REQUIRE(v.Capacity() >= v.Size());
  }
}

TEST_CASE("Move Assignment Safety", "[Safety]") {
  {
    Throwable::until_throw = 12;
    Vector<Throwable> values(5u);
    Vector<Throwable> v;
    Throwable::until_throw = 1;
    REQUIRE_NOTHROW(v = std::move(values));
  }

  {
    Throwable::until_throw = 100;
    Vector<Throwable> values(10u);
    Vector<Throwable> v(35u);
    Throwable::until_throw = 1;
    REQUIRE_NOTHROW(v = std::move(values));
  }
}

TEST_CASE("Swap Safety", "[Safety]") {
  Throwable::until_throw = 12;
  Vector<Throwable> values(5u);
  Vector<Throwable> v;
  Throwable::until_throw = 1;
  REQUIRE_NOTHROW(v.Swap(values));
}

TEST_CASE("Resize Safety", "[Safety]") {
  Throwable::until_throw = 200;
  Vector<Throwable> v(90u);
  const auto capacity = v.Capacity();
  const auto data = v.Data();

  REQUIRE_NOTHROW(v.Resize(90u));
  REQUIRE_NOTHROW(v.Resize(50u));
  REQUIRE_NOTHROW(v.Resize(10u));

  Throwable::until_throw = 10;
  REQUIRE_THROWS_AS(v.Resize(200u), Exception);
  REQUIRE(v.Size() == 10u);
  REQUIRE(v.Capacity() == capacity);
  REQUIRE(v.Data() == data);

  try {
    Throwable::until_throw = 300;
    v.Resize(200, {});
  } catch (Exception&) {
    REQUIRE(v.Size() == 10u);
    REQUIRE(v.Capacity() == capacity);
    REQUIRE(v.Data() == data);
  }
}

TEST_CASE("Reserve Safety", "[Safety]") {
  Throwable::until_throw = 55;
  Vector<Throwable> v(10u);
  REQUIRE_NOTHROW(v.Reserve(30u));

  const auto capacity = v.Capacity();
  const auto size = v.Size();
  const auto data = v.Data();
  try {
    Throwable::until_throw = 30;
    v.Reserve(100u);
  } catch (Exception&) {
    REQUIRE(v.Capacity() == capacity);
    REQUIRE(v.Size() == size);
    REQUIRE(v.Data() == data);
  }
}

TEST_CASE("ShrinkToFit Safety", "[Safety]") {
  Throwable::until_throw = 95;
  Vector<Throwable> v(20u);
  v.Reserve(30u);
  REQUIRE_NOTHROW(v.ShrinkToFit());

  v.Resize(10);
  const auto capacity = v.Capacity();
  const auto size = v.Size();
  const auto data = v.Data();
  try {
    Throwable::until_throw = 5;
    v.ShrinkToFit();
  } catch (Exception&) {
    REQUIRE(v.Capacity() == capacity);
    REQUIRE(v.Size() == size);
    REQUIRE(v.Data() == data);
  }
}

TEST_CASE("PushBack Safety", "[Safety]") {
  Throwable::until_throw = 200;
  Vector<Throwable> v;
  v.Reserve(100u);
  const auto capacity = v.Capacity();
  const auto data = v.Data();
  Throwable::until_throw = static_cast<int>(capacity) + 2;
  for (size_t i = 0; i < capacity; ++i) {
    v.PushBack({});
  }
  const Throwable object;
  REQUIRE_THROWS_AS(v.PushBack(object), Exception);
  REQUIRE(v.Size() == capacity);
  REQUIRE(v.Capacity() == capacity);
  REQUIRE(v.Data() == data);

  try {
    Throwable::until_throw = static_cast<int>(2 * capacity) + 1;
    v.PushBack(object);
  } catch (...) {
    REQUIRE(v.Size() == capacity);
    REQUIRE(v.Capacity() == capacity);
    REQUIRE(v.Data() == data);
  }
}


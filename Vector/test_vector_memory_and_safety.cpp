#include <catch.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "test_util.hpp"
#include "vector.hpp"



TEST_CASE("ConstructorsAndDestructors", "[Memory]") {
  InstanceCounter::counter = 0u;

  SECTION("Default Constructor") {
    const Vector<InstanceCounter> v;
    REQUIRE(InstanceCounter::counter == 0u);
  }

  SECTION("Size Constructor") {
    const Vector<InstanceCounter> v(10u);
    REQUIRE(InstanceCounter::counter == 10u);
  }

  SECTION("Size-Value Constructor") {
    const Vector<InstanceCounter> v(10u, InstanceCounter{});
    REQUIRE(InstanceCounter::counter == 10u);
  }

  SECTION("Iterators Constructor") {
    const std::vector<InstanceCounter> values(100u);
    const Vector<InstanceCounter> v(values.begin(), values.end());
    REQUIRE(InstanceCounter::counter == 200u);
  }

  SECTION("Initializer List Constructor") {
    const Vector<InstanceCounter> v{InstanceCounter{}, InstanceCounter{}, InstanceCounter{}};
    REQUIRE(InstanceCounter::counter == 3u);
  }

  SECTION("Copy Constructor") {
    const Vector<InstanceCounter> values(100u);
    const auto v = values;
    REQUIRE(InstanceCounter::counter == 200u);
  }

  SECTION("Move Constructor") {
    Vector<InstanceCounter> values(100u);
    const auto v = std::move(values);
    REQUIRE(InstanceCounter::counter == 100u);
  }

  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("Assignment Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  SECTION("Copy Assignment") {
    {
      const Vector<InstanceCounter> values(10u);
      Vector<InstanceCounter> v(100u);
      v = values;
      REQUIRE(InstanceCounter::counter == 20u);
    }
    REQUIRE(InstanceCounter::counter == 0u);

    {
      const Vector<InstanceCounter> values(100u);
      Vector<InstanceCounter> v(10u);
      v = values;
      REQUIRE(InstanceCounter::counter == 200u);
    }
  }

  SECTION("Move Assignment") {
    {
      Vector<InstanceCounter> values(10u);
      Vector<InstanceCounter> v(100u);
      v = std::move(values);
      REQUIRE(InstanceCounter::counter == 10u);
    }
    REQUIRE(InstanceCounter::counter == 0u);

    {
      Vector<InstanceCounter> values(100u);
      Vector<InstanceCounter> v(10u);
      v = std::move(values);
      REQUIRE(InstanceCounter::counter == 100u);
    }
  }

  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("Swap Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> a;
    Vector<InstanceCounter> b(10u);
    a.Swap(b);
    REQUIRE(InstanceCounter::counter == 10u);
  }
  REQUIRE(InstanceCounter::counter == 0u);

  {
    Vector<InstanceCounter> a(20u);
    Vector<InstanceCounter> b(10u);
    a.Swap(b);
    REQUIRE(InstanceCounter::counter == 30u);
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("Clear Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  Vector<InstanceCounter> v(10u);
  v.Clear();
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("Resize Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    v.Resize(100u);
    REQUIRE(InstanceCounter::counter == 100u);
    v.Resize(10u);
    REQUIRE(InstanceCounter::counter == 10u);
    v.Resize(1000u);
    REQUIRE(InstanceCounter::counter == 1000u);
  }
  REQUIRE(InstanceCounter::counter == 0u);

  {
    Vector<InstanceCounter> v;
    v.Resize(100u, InstanceCounter{});
    REQUIRE(InstanceCounter::counter == 100u);
    v.Resize(10u, InstanceCounter{});
    REQUIRE(InstanceCounter::counter == 10u);
    v.Resize(1000u, InstanceCounter{});
    REQUIRE(InstanceCounter::counter == 1000u);
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("Reserve Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    v.Reserve(100u);
    REQUIRE(InstanceCounter::counter == 0u);
    v.Resize(10u);
    REQUIRE(InstanceCounter::counter == 10u);
    v.Reserve(1000u);
    REQUIRE(InstanceCounter::counter == 10u);
    v.Resize(100u);
    REQUIRE(InstanceCounter::counter == 100u);
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("ShrinkToFit Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    v.Reserve(100u);
    REQUIRE(InstanceCounter::counter == 0u);
    v.Resize(10u);
    REQUIRE(InstanceCounter::counter == 10u);
    v.ShrinkToFit();
    REQUIRE(InstanceCounter::counter == 10u);

    v.Reserve(1000u);
    REQUIRE(InstanceCounter::counter == 10u);
    v.Resize(100u);
    REQUIRE(InstanceCounter::counter == 100u);
    v.ShrinkToFit();
    REQUIRE(InstanceCounter::counter == 100u);
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("PushBack Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    for (size_t i = 0; i < 100; ++i) {
      v.PushBack({});
      REQUIRE(InstanceCounter::counter == (i + 1));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);

  {
    Vector<InstanceCounter> v;
    InstanceCounter obj;
    for (size_t i = 0; i < 100; ++i) {
      v.PushBack(obj);
      REQUIRE(InstanceCounter::counter == (i + 2));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("PushBack From Itself Memory", "[Memory]") {
  {
    Vector<InstanceCounter> v;
    InstanceCounter obj;
    v.PushBack(obj);
    for (size_t i = 0; i < 100; ++i) {
      v.PushBack(v.Front());
      REQUIRE(InstanceCounter::counter == (i + 3));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);

  {
    Vector<InstanceCounter> v;
    InstanceCounter obj;
    v.PushBack(obj);
    for (size_t i = 0; i < 100; ++i) {
      v.PushBack(v.Back());
      REQUIRE(InstanceCounter::counter == (i + 3));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

TEST_CASE("EmplaceBack Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    for (size_t i = 0; i < 100; ++i) {
      v.EmplaceBack();
      REQUIRE(InstanceCounter::counter == (i + 1));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);

  {
    Vector<std::vector<int>> v;
    for (size_t i = 0; i < 10; ++i) {
      v.EmplaceBack();
      REQUIRE(v.Size() == static_cast<unsigned>(i + 1));
      REQUIRE(v.Capacity() >= v.Size());
      REQUIRE(v.Capacity() <= 2 * v.Size());
      REQUIRE(v.Back().empty());
    }
    for (size_t i = 10; i < 30; ++i) {
      v.EmplaceBack(10);
      REQUIRE(v.Size() == static_cast<unsigned>(i + 1));
      REQUIRE(v.Capacity() >= v.Size());
      REQUIRE(v.Capacity() <= 2 * v.Size());
      REQUIRE(v.Back().size() == 10u);
    }
    for (size_t i = 30; i < 70; ++i) {
      v.EmplaceBack(11, -11);
      REQUIRE(v.Size() == static_cast<unsigned>(i + 1));
      REQUIRE(v.Capacity() >= v.Size());
      REQUIRE(v.Capacity() <= 2 * v.Size());
      REQUIRE(v.Back() == std::vector<int>(11, -11));
    }
  }
}

TEST_CASE("PopBack Memory", "[Memory]") {
  InstanceCounter::counter = 0u;

  {
    Vector<InstanceCounter> v;
    for (int i = 0; i < 100; ++i) {
      v.PushBack({});
    }
    for (int i = 0; i < 50; ++i) {
      v.PopBack();
      REQUIRE(InstanceCounter::counter == static_cast<unsigned>(100 - i - 1));
    }
  }
  REQUIRE(InstanceCounter::counter == 0u);
}

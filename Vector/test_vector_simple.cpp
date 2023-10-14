#define CATCH_CONFIG_MAIN
#include <catch.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "test_util.hpp"
#include "vector.hpp"

const char* TEST_STRING = "test string";

TEST_CASE("TypeTraits") {
  REQUIRE((std::is_same_v<Vector<int>::ValueType, int>));
  REQUIRE((std::is_same_v<Vector<int>::Pointer, decltype(std::declval<Vector<int>>().Data())>));
  REQUIRE((std::is_same_v<Vector<int>::ConstPointer, decltype(std::declval<const Vector<int>>().Data())>));
  REQUIRE((std::is_same_v<Vector<int>::Reference, decltype(std::declval<Vector<int>>()[0])>));
  REQUIRE((std::is_same_v<Vector<int>::Reference, decltype(std::declval<Vector<int>>().At(0))>));
  REQUIRE((std::is_same_v<Vector<int>::Reference, decltype(std::declval<Vector<int>>().Front())>));
  REQUIRE((std::is_same_v<Vector<int>::Reference, decltype(std::declval<Vector<int>>().Back())>));
  REQUIRE((std::is_same_v<Vector<int>::ConstReference, decltype(std::declval<const Vector<int>>()[0])>));
  REQUIRE((std::is_same_v<Vector<int>::ConstReference, decltype(std::declval<const Vector<int>>().At(0))>));
  REQUIRE((std::is_same_v<Vector<int>::ConstReference, decltype(std::declval<const Vector<int>>().Front())>));
  REQUIRE((std::is_same_v<Vector<int>::ConstReference, decltype(std::declval<const Vector<int>>().Back())>));
  REQUIRE((std::is_same_v<Vector<int>::SizeType, decltype(std::declval<const Vector<int>>().Size())>));
  REQUIRE((std::is_same_v<Vector<int>::SizeType, decltype(std::declval<const Vector<int>>().Capacity())>));
}

TEST_CASE("Constructor") {
  SECTION("Default") {
    const Vector<int> v;
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
    REQUIRE(v.Data() == nullptr);
    REQUIRE(v.Empty());
  }
  SECTION("Single Parameter") {
    {
      const Vector<std::string> v(10);
      Equal(v, std::vector<std::string>(10));
      REQUIRE(v.Size() == 10);
      REQUIRE(v.Capacity() == 10);
      REQUIRE_FALSE(v.Empty());
    }
  }
  SECTION("Fill initialization") {
    SECTION("Empty") {
      const Vector<int> v(0, 10);
      REQUIRE(v.Size() == 0);
      REQUIRE(v.Capacity() == 0);
      REQUIRE(v.Data() == nullptr);
    }

    SECTION("Filled") {
      std::string str = TEST_STRING;

      const Vector<std::string> v(5u, str);
      Equal(v, std::vector<std::string>(5u, str));
      REQUIRE(v.Capacity() == 5u);
    }
  }
  SECTION("InitializerList") {
    SECTION("Empty") {
      const Vector<int> v{};
      REQUIRE(v.Size() == 0);
      REQUIRE(v.Capacity() == 0);
      REQUIRE(v.Data() == nullptr);
    }

    SECTION("Filled") {
      const Vector<int> v{1, 2, 3, 4, 5};
      Equal(v, std::vector<int>{1, 2, 3, 4, 5});
    }
  }

  SECTION("Copy Constructor") {
    SECTION("Empty") {
      const Vector<int> empty;
      const auto v = empty;
      REQUIRE(empty.Size() == 0);
      REQUIRE(empty.Capacity() == 0);
      REQUIRE(empty.Data() == nullptr);
      REQUIRE(v.Size() == 0);
      REQUIRE(v.Capacity() == 0);
      REQUIRE(v.Data() == nullptr);
    }

    SECTION("Filled") {
      const Vector<std::vector<int>> values{{1, 2}, {3, 4, 5}};
      const auto v = values;
      Equal(v, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
      Equal(values, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
    }
  }

  SECTION("Move Constructor") {
    SECTION("Empty") {
      Vector<int> empty;
      const auto v = std::move(empty);
      REQUIRE(empty.Size() == 0);
      REQUIRE(empty.Capacity() == 0);
      REQUIRE(empty.Data() == nullptr);
      REQUIRE(v.Size() == 0);
      REQUIRE(v.Capacity() == 0);
      REQUIRE(v.Data() == nullptr);
    }

    SECTION("Filled") {
      Vector<std::vector<int>> values{{1, 2}, {3, 4, 5}};
      const auto v = std::move(values);
      Equal(v, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
      REQUIRE(values.Size() == 0);
      REQUIRE(values.Capacity() == 0);
      REQUIRE(values.Data() == nullptr);
    }
  }
}

TEST_CASE("Copy Assignment", "[Assignment]") {
  SECTION("Empty to empty") {
    const Vector<int> empty;
    Vector<int> v;
    v = empty;
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
    REQUIRE(v.Data() == nullptr);
  }

  SECTION("Empty to filled") {
    const Vector<int> empty;
    Vector<int> v{1, 2, 3};
    v = empty;
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(v.Size() == 0);
  }

  SECTION("Filled to empty") {
    const Vector<int> values{1, 2, 3};
    Vector<int> v;
    v = values;
    Equal(v, std::vector<int>{1, 2, 3});
    Equal(values, std::vector<int>{1, 2, 3});

    SECTION("Self-assignment") {
      v = v;
      Equal(v, std::vector<int>{1, 2, 3});
      Equal(values, std::vector<int>{1, 2, 3});
    }
  }

  SECTION("Small to large") {
    Vector<int> large(1000, 1);
    const Vector<int> small{1, 2, 3};
    large = small;
    Equal(large, std::vector<int>{1, 2, 3});
    Equal(small, std::vector<int>{1, 2, 3});
  }

  SECTION("Large to small") {
    const Vector<int> large(1000, 1);
    Vector<int> small{1, 2, 3};
    small = large;
    Equal(large, std::vector<int>(1000, 1));
    Equal(small, std::vector<int>(1000, 1));
  }

  SECTION("Deep copy") {
    const Vector<std::vector<int>> values{{1, 2}, {3, 4, 5}};
    Vector<std::vector<int>> v;
    v = values;
    Equal(v, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
    Equal(values, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
    SECTION("Self-assignment") {
      v = v;
      Equal(v, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
      Equal(values, std::vector<std::vector<int>>{{1, 2}, {3, 4, 5}});
    }
  }
}

TEST_CASE("Move Assignment", "[Assignment]") {
  SECTION("Empty to empty") {
    Vector<int> empty;
    Vector<int> v;

    v = std::move(empty);
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
    REQUIRE(v.Data() == nullptr);

    v = Vector<int>{};
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
    REQUIRE(v.Data() == nullptr);
  }

  SECTION("Empty to filled") {
    Vector<int> empty;
    Vector<int> v{1, 2, 3};
    v = std::move(empty);
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(v.Size() == 0);

    v = Vector<int>{1, 2, 3};
    v = Vector<int>{};
    REQUIRE(v.Size() == 0);
  }

  SECTION("Filled to empty") {
    Vector<int> values{1, 2, 3};
    const auto p_values = values.Data();
    Vector<int> v;
    v = std::move(values);
    Equal(v, std::vector<int>{1, 2, 3});
    REQUIRE(p_values == v.Data());
    REQUIRE(values.Size() == 0);
    REQUIRE(values.Capacity() == 0);
    REQUIRE(values.Data() == nullptr);

    v = Vector<int>{4, 5, 6};
    Equal(v, std::vector<int>{4, 5, 6});
    REQUIRE(p_values != v.Data());
  }

  SECTION("Small to large") {
    Vector<int> large(1000, 1);
    Vector<int> small{1, 2, 3};
    const auto p_small = small.Data();
    large = std::move(small);
    Equal(large, std::vector<int>{1, 2, 3});
    REQUIRE(p_small == large.Data());
    REQUIRE(small.Size() == 0);
    REQUIRE(small.Capacity() == 0);
    REQUIRE(small.Data() == nullptr);
  }

  SECTION("Large to small") {
    Vector<int> large(1000, 1);
    Vector<int> small{1, 2, 3};
    const auto p_large = large.Data();
    small = std::move(large);
    Equal(small, std::vector<int>(1000, 1));
    REQUIRE(p_large == small.Data());
    REQUIRE(large.Size() == 0);
    REQUIRE(large.Capacity() == 0);
    REQUIRE(large.Data() == nullptr);
  }
}

TEST_CASE("DataAccess", "[Methods]") {
  Vector<int> v{1, 2, 3, 4, 5};

  SECTION("Front") {
    REQUIRE(v.Front() == 1);
    v.Front() = -1;
    REQUIRE(std::as_const(v).Front() == -1);
  }

  SECTION("Back") {
    REQUIRE(v.Back() == 5);
    v.Back() = -5;
    REQUIRE(std::as_const(v).Back() == -5);
  }

  SECTION("Operator []") {
    REQUIRE(v[1] == 2);
    v[1] = -2;
    REQUIRE(std::as_const(v)[1] == -2);
  }

  SECTION("At") {
    REQUIRE(v.At(2) == 3);
    v.At(2) = -3;
    REQUIRE(std::as_const(v).At(2) == -3);
    REQUIRE_THROWS_AS(v.At(5), std::out_of_range);
    REQUIRE_THROWS_AS(std::as_const(v).At(5), std::out_of_range);
  }
}

TEST_CASE("Swap", "[Methods]") {
  SECTION("Empty to empty") {
    Vector<int> a;
    Vector<int> b;
    a.Swap(b);
    REQUIRE(a.Data() == nullptr);
    REQUIRE(a.Size() == 0);
    REQUIRE(a.Capacity() == 0);
    REQUIRE(b.Data() == nullptr);
    REQUIRE(b.Size() == 0);
    REQUIRE(b.Capacity() == 0);
  }

  SECTION("Empty to filled") {
    Vector<int> a;
    Vector<int> b{1, 2, 3};
    const auto pb = b.Data();
    a.Swap(b);
    Equal(a, std::vector<int>{1, 2, 3});
    REQUIRE(a.Data() == pb);
    REQUIRE(b.Data() == nullptr);
    REQUIRE(b.Size() == 0);
    REQUIRE(b.Capacity() == 0);
  }

  SECTION("Small to large") {
    Vector<int> large(1000, 1);
    Vector<int> small{1, 2, 3};
    const auto p_small = small.Data();
    const auto p_large = large.Data();
    small.Swap(large);
    Equal(small, std::vector<int>(1000, 1));
    Equal(large, std::vector<int>{1, 2, 3});
    REQUIRE(small.Data() == p_large);
    REQUIRE(large.Data() == p_small);
  }
}

TEST_CASE("Clear", "[Methods]") {
  SECTION("Empty") {
    Vector<int> empty;
    empty.Clear();
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
  }

  SECTION("Filled") {
    Vector<int> v(1000, 1);
    v.Clear();
    REQUIRE(v.Capacity() > 0);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Empty());
  }

  SECTION("Assigned") {
    Vector<std::unique_ptr<int>> v(2);
    v[0] = std::make_unique<int>(1);
    v[1] = std::make_unique<int>(2);
    v.Clear();
    REQUIRE(v.Capacity() > 0);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Empty());
  }
}

TEST_CASE("Resize", "[Methods]") {
  SECTION("Empty to 0") {
    Vector<std::unique_ptr<int>> v;
    v.Resize(0);
    REQUIRE(v.Data() == nullptr);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
  }

  SECTION("Empty to positive size") {
    Vector<int> v;
    SECTION("Only resize") {
      v.Resize(5);
      Equal(v, std::vector<int>(5));
      REQUIRE(v.Capacity() >= 5);
      REQUIRE(v.Capacity() <= 10);
    }
    SECTION("Fill with arg") {
      v.Resize(5, 1);
      Equal(v, std::vector<int>(5, 1));
      REQUIRE(v.Capacity() >= 5);
      REQUIRE(v.Capacity() <= 10);
    }
  }

  SECTION("No reductunt realloc") {
    Vector<int> v(10, 1);
    const auto pv = v.Data();
    v.Resize(10);
    Equal(v, std::vector<int>(10, 1));
    REQUIRE(pv == v.Data());
  }

  SECTION("Filled to positive size") {
    Vector<int> v(100, 1);
    SECTION("Only resize") {
      REQUIRE(v.Capacity() < 1000);
      v.Resize(1000);

      REQUIRE(v.Size() == 1000);
      REQUIRE(v.Capacity() >= 1000);
      REQUIRE(v.Capacity() <= 2000);
      for (int i = 0; i < 100; ++i) {
        REQUIRE(v[i] == 1);
      }
      for (int i = 100; i < 1000; ++i) {
        REQUIRE(v[i] == 0);
      }
    }
    SECTION("Fill with arg") {
      REQUIRE(v.Capacity() < 1000);
      v.Resize(1000, 2);

      REQUIRE(v.Size() == 1000);
      REQUIRE(v.Capacity() >= 1000);
      REQUIRE(v.Capacity() <= 2000);
      for (int i = 0; i < 100; ++i) {
        REQUIRE(v[i] == 1);
      }
      for (int i = 100; i < 1000; ++i) {
        REQUIRE(v[i] == 2);
      }
    }
  }

  SECTION("Multy-resize") {
    Vector<int> v(1000, 1);
    const auto pv = v.Data();

    v.Resize(400, -1);
    Equal(v, std::vector<int>(400, 1));
    REQUIRE(pv == v.Data());

    v.Resize(100);
    Equal(v, std::vector<int>(100, 1));
    REQUIRE(pv == v.Data());

    v.Resize(500, -1);
    for (int i = 0; i < 100; ++i) {
      REQUIRE(v[i] == 1);
    }
    for (int i = 100; i < 500; ++i) {
      REQUIRE(v[i] == -1);
    }
    REQUIRE(v.Size() == 500);
    REQUIRE(pv == v.Data());

    v.Resize(750);
    for (int i = 0; i < 100; ++i) {
      REQUIRE(v[i] == 1);
    }
    for (int i = 100; i < 500; ++i) {
      REQUIRE(v[i] == -1);
    }
    REQUIRE(v.Size() == 750);
    REQUIRE(pv == v.Data());
  }
}

TEST_CASE("Reserve", "[Methods]") {
  SECTION("Empty to 0") {
    Vector<std::unique_ptr<int>> v;
    v.Reserve(0);
    REQUIRE(v.Data() == nullptr);
    REQUIRE(v.Size() == 0);
    REQUIRE(v.Capacity() == 0);
  }

  SECTION("Empty to positive size") {
    Vector<std::unique_ptr<int>> empty;
    empty.Reserve(10);
    Equal(empty, std::vector<std::unique_ptr<int>>{});
    REQUIRE(empty.Capacity() >= 10);
    REQUIRE(empty.Data() != nullptr);
  }

  SECTION("No reductunt realloc") {
    Vector<int> v(10, 5);
    const auto pv = v.Data();

    v.Reserve(10);
    Equal(v, std::vector<int>(10, 5));
    REQUIRE(pv == v.Data());

    v.Reserve(5u);
    Equal(v, std::vector<int>(10, 5));
    REQUIRE(pv == v.Data());
  }

  SECTION("Save values") {
    Vector<int> v(100);
    for (int i = 0; i < 100; ++i) {
      v[i] = i;
    }
    REQUIRE(v.Capacity() < 1000);
    v.Reserve(1000);

    REQUIRE(v.Size() == 100);
    REQUIRE(v.Capacity() >= 1000);
    REQUIRE(v.Capacity() <= 2000);
    for (int i = 0; i < 100; ++i) {
      REQUIRE(v[i] == i);
    }
  }
}

TEST_CASE("ShrinkToFit", ) {
  SECTION("Empty") {
    Vector<int> empty;
    empty.ShrinkToFit();
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
  }

  SECTION("Zero-sized") {
    Vector<std::unique_ptr<int>> empty;
    empty.Reserve(10);
    empty.ShrinkToFit();
    REQUIRE(empty.Data() == nullptr);
    REQUIRE(empty.Size() == 0);
    REQUIRE(empty.Capacity() == 0);
  }

  SECTION("Sized") {
    Vector<int> v(10);
    for (int i = 0; i < 10; ++i) {
      v[i] = i;
    }
    v.Reserve(100);
    v.ShrinkToFit();
    for (int i = 0; i < 10; ++i) {
      REQUIRE(v[i] == i);
    }
    REQUIRE(v.Size() == 10);
    REQUIRE(v.Capacity() == 10);
  }

  SECTION("Realloc") {
    Vector<int> v(10, 10);
    v.Reserve(1000);
    const auto pv = v.Data();
    v.ShrinkToFit();
    Equal(v, std::vector<int>(10, 10));
    REQUIRE(v.Capacity() == 10);
    REQUIRE(pv != v.Data());
  }
}

TEST_CASE("PushBack", "[Methods]") {
  SECTION("Trivial type") {
    Vector<int> v(10);
    using VecType = Vector<int>;
    for (int i = 0; i < 10; ++i) {
      v[i] = i;
    }
    for (int i = 10; i < 100; ++i) {
      v.PushBack(i);
      REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(i + 1));
      REQUIRE(v.Capacity() >= v.Size());
      REQUIRE(v.Capacity() <= 2 * v.Size());
    }
    for (int i = 0; i < 100; ++i) {
      REQUIRE(v[i] == i);
    }
  }

  SECTION("Class") {
    Vector<std::unique_ptr<int>> v;
    using VecType = Vector<std::unique_ptr<int>>;
    for (int i = 0; i < 100; ++i) {
      v.PushBack(std::make_unique<int>(i));
      REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(i + 1));
      REQUIRE(v.Capacity() >= v.Size());
      REQUIRE(v.Capacity() <= 2 * v.Size());
    }
    for (int i = 0; i < 100; ++i) {
      REQUIRE(*v[i] == i);
    }
  }
}

TEST_CASE("PopBack", "[ReallocationStrategy]") {
  SECTION("Trivial type") {
    using VecType = Vector<int>;
    Vector<int> v(10);
    for (int i = 0; i < 10; ++i) {
      v[i] = i;
    }
    for (int i = 10; i < 100; ++i) {
      v.PushBack(i);
    }
    const auto capacity = v.Capacity();
    for (int i = 0; i < 50; ++i) {
      v.PopBack();
      REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(100 - i - 1));
    }
    REQUIRE(capacity == v.Capacity());
    for (int i = 0; i < 50; ++i) {
      REQUIRE(v[i] == i);
    }
  }
  SECTION("Class") {
    using VecType = Vector<std::unique_ptr<int>>;
    Vector<std::unique_ptr<int>> v;
    for (int i = 0; i < 100; ++i) {
      v.PushBack(std::make_unique<int>(i));
    }
    const auto capacity = v.Capacity();
    for (int i = 0; i < 50; ++i) {
      v.PopBack();
      REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(100 - i - 1));
    }
    REQUIRE(capacity == v.Capacity());
    for (int i = 0; i < 50; ++i) {
      REQUIRE(*v[i] == i);
    }
  }
}

TEST_CASE("Stress", "[ReallocationStrategy]") {
  Vector<std::unique_ptr<int>> v;
  using VecType = Vector<std::unique_ptr<int>>;
  for (int i = 0; i < 1'000'000; ++i) {
    v.PushBack(std::make_unique<int>(i));
    REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(i + 1));
    REQUIRE(v.Capacity() >= v.Size());
    REQUIRE(v.Capacity() <= 2 * v.Size());
  }
  const auto capacity = v.Capacity();
  for (int i = 0; i < 500'000; ++i) {
    v.PopBack();
    REQUIRE(v.Size() == static_cast<typename VecType::SizeType>(1'000'000 - i - 1));
  }
  REQUIRE(capacity == v.Capacity());
  for (int i = 0; i < 500'000; ++i) {
    REQUIRE(*v[i] == i);
  }
}


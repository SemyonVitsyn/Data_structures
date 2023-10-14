#include <catch.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>

#include "test_util.hpp"
#include "vector.hpp"


TEST_CASE("Constructor from iterators") {
  SECTION("Pointer") {
    const int arr[] = {1, 2, 3, 4};
    SECTION("Empty") {
      const Vector<int> v(arr, arr);
      REQUIRE(v.Size() == 0u);
      REQUIRE(v.Capacity() == 0u);
      REQUIRE(v.Data() == nullptr);
    }
    SECTION("Real range") {
      const Vector<int> v(arr + 1, arr + 4);
      Equal(v, std::vector<int>(arr + 1, arr + 4));
    }
  }

  SECTION("Iter adapter") {
    std::vector<std::unique_ptr<int>> p;
    p.push_back(std::make_unique<int>(1));
    p.push_back(std::make_unique<int>(2));

    const Vector<std::unique_ptr<int>> v(std::make_move_iterator(p.begin()), std::make_move_iterator(p.end()));
    REQUIRE(*v[0] == 1);
    REQUIRE(*v[1] == 2);
    REQUIRE(p == std::vector<std::unique_ptr<int>>(2));
  }
}

TEST_CASE("Iterator") {
  SECTION("Type traits") {
    using Iterator = Vector<int>::Iterator;
    REQUIRE((std::is_same_v<Iterator, decltype(std::declval<Vector<int>>().begin())>));
    REQUIRE((std::is_same_v<Iterator, decltype(std::declval<Vector<int>>().end())>));

    using Traits = std::iterator_traits<Iterator>;
    REQUIRE((std::is_same_v<Traits::value_type, int>));
    REQUIRE((std::is_same_v<Traits::reference, decltype(*std::declval<Iterator>())>));
    REQUIRE((std::is_base_of_v<std::random_access_iterator_tag, Traits::iterator_category>));
  }

  SECTION("Range-based for") {
    Vector<int> v(10);
    int i = 0;
    for (auto& x : v) {
      x = ++i;
    }
    i = 0;
    for (auto& x : v) {
      REQUIRE(x == ++i);
    }
  }

  SECTION("Iterator usage") {
    Vector<int> v(10, -1);
    std::fill(v.begin() + 5, v.end(), 1);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(v[i] == -1);
    }
    for (int i = 5; i < 10; ++i) {
      REQUIRE(v[i] == 1);
    }
  }
}

TEST_CASE("ConstIterator") {
  SECTION("Type traits") {
    using ConstIterator = Vector<int>::ConstIterator;
    REQUIRE((std::is_same_v<ConstIterator, decltype(std::declval<Vector<int>>().cbegin())>));
    REQUIRE((std::is_same_v<ConstIterator, decltype(std::declval<Vector<int>>().cend())>));
    REQUIRE((std::is_same_v<ConstIterator, decltype(std::declval<const Vector<int>>().begin())>));
    REQUIRE((std::is_same_v<ConstIterator, decltype(std::declval<const Vector<int>>().end())>));

    using Traits = std::iterator_traits<ConstIterator>;
    REQUIRE((std::is_same_v<Traits::value_type, int>));
    REQUIRE((std::is_same_v<Traits::reference, decltype(*std::declval<ConstIterator>())>));
    REQUIRE((std::is_base_of_v<std::random_access_iterator_tag, Traits::iterator_category>));
  }

  SECTION("Range-based for") {
    Vector<int> v(10);

    for (int64_t i = 0; auto& x : v) {
      x = ++i;
    }

    int i = 0;
    for (auto& x : std::as_const(v)) {
      REQUIRE(x == ++i);
    }

    i = 0;
    for (auto it = v.cbegin(); it != v.cend(); ++it) {
      REQUIRE(*it == ++i);
    }
  }
}

TEST_CASE("ReverseIterator") {
  SECTION("Type traits") {
    using ReverseIterator = Vector<int>::ReverseIterator;
    REQUIRE((std::is_same_v<ReverseIterator, decltype(std::declval<Vector<int>>().rbegin())>));
    REQUIRE((std::is_same_v<ReverseIterator, decltype(std::declval<Vector<int>>().rend())>));
    REQUIRE((std::is_same_v<ReverseIterator, std::reverse_iterator<Vector<int>::Iterator>>));

    using Traits = std::iterator_traits<ReverseIterator>;
    REQUIRE((std::is_same_v<Traits::value_type, int>));
    REQUIRE((std::is_same_v<Traits::reference, decltype(*std::declval<ReverseIterator>())>));
    REQUIRE((std::is_base_of_v<std::random_access_iterator_tag, Traits::iterator_category>));
  }

  SECTION("Iteration over container") {
    Vector<int> v(10);
    int i = 0;
    for (auto it = v.rbegin(); it != v.rend(); ++it) {
      *it = ++i;
    }
    i = 11;
    for (auto& x : v) {
      REQUIRE(x == --i);
    }
  }

  SECTION("Iterator usage") {
    Vector<int> v(10, -1);
    std::fill(v.rbegin() + 5, v.rend(), 1);
    for (int i = 0; i < 5; ++i) {
      REQUIRE(v[i] == 1);
    }
    for (int i = 5; i < 10; ++i) {
      REQUIRE(v[i] == -1);
    }
  }
}

TEST_CASE("ConstReverseIterator") {
  SECTION("Type traits") {
    using ConstReverseIterator = Vector<int>::ConstReverseIterator;
    REQUIRE((std::is_same_v<ConstReverseIterator, decltype(std::declval<Vector<int>>().crbegin())>));
    REQUIRE((std::is_same_v<ConstReverseIterator, decltype(std::declval<Vector<int>>().crend())>));
    REQUIRE((std::is_same_v<ConstReverseIterator, decltype(std::declval<const Vector<int>>().rbegin())>));
    REQUIRE((std::is_same_v<ConstReverseIterator, decltype(std::declval<const Vector<int>>().rend())>));
    REQUIRE((std::is_same_v<ConstReverseIterator, std::reverse_iterator<Vector<int>::ConstIterator>>));

    using Traits = std::iterator_traits<ConstReverseIterator>;
    REQUIRE((std::is_same_v<Traits::value_type, int>));
    REQUIRE((std::is_same_v<Traits::reference, decltype(*std::declval<ConstReverseIterator>())>));
    REQUIRE((std::is_base_of_v<std::random_access_iterator_tag, Traits::iterator_category>));
  }

  SECTION("Iterator usage") {
    Vector<int> v(10);
    for (int64_t i = 11; auto& x : v) {
      x = --i;
    }
    int i = 0;
    for (auto it = std::as_const(v).rbegin(); it != std::as_const(v).rend(); ++it) {
      REQUIRE(*it == ++i);
    }

    i = 0;
    for (auto it = v.crbegin(); it != v.crend(); ++it) {
      REQUIRE(*it == ++i);
    }
  }
}

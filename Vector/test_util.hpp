#pragma once
#include <vector>
#include "vector.hpp"
#include <catch.hpp>
#include <stdexcept>

template <class T>
void Equal(const Vector<T>& real, const std::vector<T>& required) {
  REQUIRE(real.Size() == required.size());
  for (size_t i = 0u; i < real.Size(); ++i) {
    REQUIRE(real[i] == required[i]);
  }
}

struct InstanceCounter {
  static size_t counter;

  InstanceCounter() noexcept {
    ++counter;
  }

  InstanceCounter(const InstanceCounter&) : InstanceCounter() {
  }

  InstanceCounter(InstanceCounter&&) : InstanceCounter() {
  }

  InstanceCounter& operator=(const InstanceCounter&) = default;
  InstanceCounter& operator=(InstanceCounter&&) noexcept = default;

  ~InstanceCounter() {
    --counter;
  }
};



class Exception : std::exception {};

class Throwable {
  std::unique_ptr<int> p_ = std::make_unique<int>();

 public:
  static int until_throw;
  int marker = 0;

  Throwable() {
    --until_throw;
    if (until_throw <= 0) {
      throw Exception{};
    }
  }

  Throwable(const Throwable&) : Throwable() {
  }

  Throwable(Throwable&&) noexcept = default;

  Throwable& operator=(const Throwable&) {
    --until_throw;
    if (until_throw <= 0) {
      throw Exception{};
    }
    return *this;
  }

  Throwable& operator=(Throwable&&) noexcept = default;
};



class MoveThrowable : public Throwable {
 public:
  static bool was_moved;
  MoveThrowable() : Throwable::Throwable() {
    was_moved = false;
  }
  MoveThrowable(const Throwable&) : Throwable::Throwable() {
  }
  MoveThrowable(Throwable&&) : Throwable::Throwable() {
    was_moved = true;
  }
};

#include "test_util.hpp"

bool MoveThrowable::was_moved = false;

int Throwable::until_throw = std::numeric_limits<int>::max();

size_t InstanceCounter::counter = 0u;

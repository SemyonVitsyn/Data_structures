#include "complex.hpp"
#include <cmath>


template<typename T>
T Abs(const T number) {
    return number > 0 ? number : -number;
}

template<>
Complex Abs<Complex>(const Complex number) {
    return sqrt(number.GetRe() * number.GetRe() + number.GetIm() * number.GetIm());
}

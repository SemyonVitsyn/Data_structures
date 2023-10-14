#pragma once
#include <stdexcept>

template<typename T, int N>
class Array {

public:
    Array() = default;


    ~Array() = default;


    Array(std::initializer_list<T> lst) {  // Do not touch.
        std::copy(lst.begin(), lst.end(), arr_);
    }


    T& operator[](int i) {
        return arr_[i];
    } 

    const T& operator[](int i) const {
        return const_cast<const T&>(const_cast< Array<T, N>* >(this)->operator[](i));
    }


    T& Front() {
        return arr_[0];
    }

    const T& Front() const {
        return const_cast<const T&>(const_cast< Array<T, N>* >(this)->Front());
    }


    T& Back() {
        return arr_[N-1];
    }

    const T& Back() const {
        return const_cast<const T&>(const_cast< Array<T, N>* >(this)->Back());
    }


    T* Data() {
        return arr_;
    }

    const T* Data() const {
        return arr_;
    }


    int Size() const {
        return N;
    }


    int Empty() const {
        return (Size() == 0);
    } 


    void Swap(Array<T, N>& cur) {
        for (int i = 0; i < N; ++i)
            std::swap(arr_[i], cur.arr_[i]);
    }

private:
    T arr_[N];

};

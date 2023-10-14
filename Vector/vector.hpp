#pragma once

#include <stdexcept>
#include <exception>

template <typename T>
class Vector {

public:

using ValueType = T;
using Pointer = T*;
using ConstPointer = const T*;
using Reference = T&;
using ConstReference = const T&;
using SizeType = size_t;
using Iterator = T*;
using ConstIterator = const T*;
using ReverseIterator = std::reverse_iterator<Iterator>;
using ConstReverseIterator = std::reverse_iterator<ConstIterator>;

    
//------------------------------constructors------------------------------

    Vector() : data_(nullptr), size_(0), capacity_(0) {}

    Vector(size_t size) 
        : size_(size), capacity_(size) 
    {
        if (size > 0) {
            size_t idx = 0;
            try {
                data_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
                for (idx = 0; idx < size; ++idx)
                    new (data_ + idx) T();
            }
            catch (...) {
                for (size_t i = 0; i < idx; ++i)
                    (data_ + i)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;

                throw;
            }
        }
    }

    Vector(size_t size, const T& elem)
        : size_(size), capacity_(size)
    {
        if (size > 0) {
            size_t idx = 0;
            try {
                data_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
                for (idx = 0; idx < size; ++idx)
                    new (data_ + idx) T(elem);
            }
            catch (...) {
                for (size_t i = 0; i < idx; ++i)
                    (data_ + i)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;

                throw;
            }
        }
    }

    Vector(const std::initializer_list<T>& list) {
        int size = list.size();
        size_ = size;
        capacity_ = size;
        if (size > 0) {
            size_t idx = 0;
            try {
                data_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
                for (auto it = list.begin(); it != list.end(); ++it, ++idx)
                    new (data_ + idx) T(*it);
            }
            catch (...) {
                for (size_t i = 0; i < idx; ++i)
                    (data_ + i)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;

                throw;
            }
        }
    }

    Vector(const Vector& other) 
        : size_(other.size_), capacity_(other.capacity_)
    {
        if (other.data_ != nullptr) {
            size_t idx = 0;
            try {
                data_ = reinterpret_cast<T*>(new int8_t[other.capacity_ * sizeof(T)]);
                for (idx = 0; idx < other.size_; ++idx) {
                    new (data_ + idx) T(other.data_[idx]);
                }
            }
            catch (...) {
                for (size_t i = 0; i < idx; ++i)
                    (data_ + i)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;

                throw;
            }
        } 
    }

    Vector(Vector&& other) noexcept {
        Swap(other);
    }

    template<typename InputIterator,
    typename = typename std::iterator_traits<InputIterator>::iterator_category>
    Vector(InputIterator begin, InputIterator end) {
        if (begin != end) {
            try {
                while (begin != end) {
                    PushBack(*begin);
                    ++begin;
                }

                ShrinkToFit();
            }
            catch(...) {
                for (size_t i = 0; i < size_; ++i)
                    (data_ + i)->~T();
                delete[] reinterpret_cast<int8_t*>(data_);
                data_ = nullptr;

                throw;
            }
        }
    }

//-----------------------------destructor--------------------------------

    ~Vector() {
        if (data_ != nullptr) {
            for (size_t i = 0; i < size_; ++i) {
                (data_ + i)->~T();
            }
        }

        delete[] reinterpret_cast<int8_t*>(data_);
    }

//-----------------------------iterators--------------------------------

    Iterator begin() {
        return data_;
    }

    ConstIterator begin() const {
        return data_;
    }

    Iterator end() {
        return data_ + size_;
    }

    ConstIterator end() const {
        return data_ + size_;
    }

    ConstIterator cbegin() const {
        return data_;
    }

    ConstIterator  cend() const {
        return data_ + size_;
    }

    ReverseIterator rbegin() {
        return std::reverse_iterator(end());
    }

    ConstReverseIterator rbegin() const {
        return std::reverse_iterator(end());
    }

    ConstReverseIterator crbegin() const {
        return rbegin();
    }

    ReverseIterator rend() {
        return std::reverse_iterator(begin());
    }

    ConstReverseIterator rend() const {
        return std::reverse_iterator(begin());
    }

    ConstReverseIterator crend() const {
        return rend();
    }

//-----------------------------operators--------------------------------

    Vector& operator=(const Vector& other) {
        if (this == &other)
            return *this;

        Vector temp = other;
        Swap(temp);

        return *this;
    }

    Vector& operator=(Vector&& other) noexcept {
        if (this == &other)
            return *this;

        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
        delete[] reinterpret_cast<int8_t*>(data_);
        data_ = nullptr;
        size_ = 0;
        capacity_ = 0;

        Swap(other);

        return *this;
    }

    T& operator[](size_t idx) {
        return data_[idx];
    }

    const T& operator[](size_t idx) const {
        return data_[idx];
    }

//-----------------------------methods----------------------------------

    void Swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    T* Data() {
        return data_;
    }    

    const T* Data() const {
        return data_;
    }

    T& Front() {
        return data_[0];
    }

    const T& Front() const {
        return data_[0];
    }

    T& Back() {
        return data_[size_ - 1];
    }

    const T& Back() const {
        return data_[size_ - 1];
    }

    T& At(size_t idx) {
        if (idx >= size_)
            throw std::out_of_range("Out of range");
        return data_[idx];
    }

    const T& At(size_t idx) const {
        if (idx >= size_)
            throw std::out_of_range("Out of range");
        return data_[idx];
    }

    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return capacity_;
    }

    bool Empty() const {
        return size_ == 0;
    }

    void Clear() {
        for (size_t i = 0; i < size_; ++i) {
            (data_ + i)->~T();
        }
        size_ = 0;
    }

    void Resize(size_t size) {
        if (size <= size_) {
            for (size_t i = size; i < size_; ++i) {
                (data_ + i)->~T();
            }
            size_ = size;

            return;
        }

        T* new_data_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
        size_t idx = 0;
        try {
            for (idx = 0; idx < size - size_; ++idx) {
                new (new_data_ + idx) T();
            }
        }
        catch (...) {
            for (size_t i = 0; i < idx; ++i)
                (new_data_ + i)->~T();
            delete[] reinterpret_cast<int8_t*>(new_data_);
                
            throw;
        }

        if (size > capacity_) {
            Reserve(size);
        }
        try {
            for (idx = size_; idx < size; ++idx) {
                new (data_ + idx) T();
            }
        }
        catch (...) {
                for (size_t i = size_; i < idx; ++i)
                    (data_ + i)->~T();
                for (size_t i = 0; i < size - size_; ++i) {
                    (new_data_ + i)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(new_data_);
                throw;
        }

        for (size_t i = 0; i < size - size_; ++i)   
            (new_data_ + i)->~T();

        delete[] reinterpret_cast<int8_t*>(new_data_);
        size_ = size;
    }

    void Resize(size_t size, const T& elem) {
        if (size <= size_) {
            for (size_t i = size; i < size_; ++i) {
                (data_ + i)->~T();
            }
            size_ = size;

            return;
        }

        T* new_data_ = reinterpret_cast<T*>(new int8_t[size * sizeof(T)]);
        size_t idx = 0;
        try {
            for (idx = 0; idx < size - size_; ++idx) {
                new (new_data_ + idx) T(std::move_if_noexcept(elem));
            }
        }
        catch (...) {
            for (size_t i = 0; i < idx; ++i)
                (new_data_ + i)->~T();
            delete[] reinterpret_cast<int8_t*>(new_data_);
                
            throw;
        }

        if (size > capacity_) {
            Reserve(size);
        }
        try {
            for (idx = size_; idx < size; ++idx) {
                new (data_ + idx) T(std::move_if_noexcept(new_data_[idx-size_]));
            }
        }
        catch (...) {
                for (size_t i = size_; i < idx; ++i)
                    (data_ + i)->~T();
                for (size_t i = 0; i < size - size_; ++i) {
                    (new_data_ + i)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(new_data_);
                throw;
        }

        for (size_t i = 0; i < size - size_; ++i)   
            (new_data_ + i)->~T();

        delete[] reinterpret_cast<int8_t*>(new_data_);
        size_ = size;
    }

    void Reserve(size_t capacity) {
        if (capacity_ >= capacity)
            return;

        T* new_data_ = reinterpret_cast<T*>(new int8_t[capacity * sizeof(T)]);
        size_t idx = 0;

        for (idx = 0; idx < size_; ++idx) {
            try {
                new (new_data_ + idx) T(std::move_if_noexcept(data_[idx]));
            }
            catch (...) {
                for (size_t i = 0; i < idx; ++i) {
                    (new_data_ + i)->~T();
                }
                delete[] reinterpret_cast<int8_t*>(new_data_);
                new_data_ = nullptr;
                throw;
            }
        }    

        for (size_t i = 0; i < size_; ++i) 
            (data_ + i)->~T();
        
        delete[] reinterpret_cast<int8_t*>(data_);
        data_ = new_data_;
        capacity_ = capacity;
    }

    void ShrinkToFit() {
        Vector new_v(size_);
        size_t idx = 0;
        try {
            for (idx = 0; idx < size_; ++idx)
                new_v[idx] = std::move_if_noexcept(data_[idx]);
        }
        catch (...) {
            for (size_t i = 0; i < idx; ++i) {
                new_v.PopBack();
            }
        }

        Swap(new_v);
    }

    template<typename... Args>
    void EmplaceBack(Args... args) {
        if (size_ == capacity_) {
            Reserve(std::max(static_cast<size_t>(1), capacity_ * 2));
        }

        new (data_ + size_) T(std::forward<Args>(args)...);
        size_++;
    }

    void PushBack(const T& elem) {
        EmplaceBack(elem);
    }

    void PushBack(T&& elem) {
        EmplaceBack(std::forward<T>(elem));
    }

    void PopBack() {
        if (size_ > 0) {
            (data_ + size_ - 1)->~T();
            size_--;
        }
    }

private:
    T* data_ = nullptr;
    size_t size_ = 0;
    size_t capacity_ = 0;
};

#pragma once

#include <cstring>
#include <algorithm>
#include <sstream>
#include <compare>

class String {

public:

    String();
    String(const std::size_t size, const char symbol);
    String(const char* str, const std::size_t size);
    String(const char* str);
    String(const String& str);

    ~String();

    char& Front();
    const char& Front() const;

    char& Back();
    const char& Back() const;

    const char* CStr() const;
    const char* Data() const;

    bool Empty() const;

    size_t Size() const; 

    size_t Length() const;

    size_t Capacity() const;

    void Clear();

    void Swap(String& other_str);

    void PopBack();

    void PushBack(const char symbol);

    void Resize(const std::size_t new_size, const char symbol);

    void Reserve(const std::size_t new_capacity);

    void ShrinkToFit();

    char& operator[](const std::size_t idx);

    const char& operator[](const std::size_t idx) const;

    String& operator=(const String& str);

    String& operator+=(const String& str);

    String operator+(const String& str) const;

    bool operator==(const String& str) const;

private:
    char* _data = nullptr;
    std::size_t _size = 0;
    std::size_t _capacity = 0;

};


int operator<=>(const String& str1, const String& str2);

std::ostream& operator<<(std::ostream& out, const String& str);

std::istream& operator>>(std::istream& in, String& str);

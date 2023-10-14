#include "str.hpp"


//------------------------constructors-------------------------

String::String() = default;

String::String(const std::size_t size, const char symbol) {
    if (size != 0) {
        _size = size;
        _capacity = _size;
        _data = new char[_capacity + 1]();
        std::memset(_data, symbol, _size);
    }
}

String::String(const char* str, const std::size_t size) {
    if (size != 0) {
        _size = size;
        _capacity = _size;
        _data = new char[_capacity + 1];
        std::memcpy(_data, str, _size);
    }
}

String::String(const char* str) : String(str, strlen(str)) {}

String::String(const String& str) {
    *this = str;    
}

//------------------------destructor---------------------------

String::~String() {
    delete[] _data;
}

//---------------------------get-------------------------------

char& String::Front() {
    return _data[0];
}

const char& String::Front() const {
    return _data[0];
}


char& String::Back() {
    return _data[_size-1];
}

const char& String::Back() const {
    return _data[_size-1];
}


const char* String::CStr() const {
    _data[_size] = '\0';
    return _data;
}


const char* String::Data() const {
    return _data;
}


bool String::Empty() const {
    return _size == 0;
}


size_t String::Size() const {
    return _size;
}


size_t String::Length() const {
    return _size;
}


size_t String::Capacity() const {
    return _capacity;
}

//-------------------------change------------------------------

void String::Clear() {
    _size = 0;
}

void String::Swap(String& other_str) {
    std::swap(_size, other_str._size);
    std::swap(_capacity, other_str._capacity);
    std::swap(_data, other_str._data);
}

void String::PopBack() {
    if (_size > 0) {
        _size--;
}
}

void String::PushBack(const char symbol) {
    if (_size + 1 >= _capacity) {
        if (_capacity != 0)
            Reserve(_capacity * 2);
        else
            Reserve(1);
    }
    
    _data[_size++] = symbol;
}

void String::Resize(const std::size_t new_size, const char symbol) {
    if (new_size > _capacity) 
        Reserve(new_size);

    if (new_size > _size)
        std::memset(_data + _size, symbol, new_size - _size);

    _size = new_size;
}

void String::Reserve(const std::size_t new_capacity) {
    if (_capacity >= new_capacity)
        return;

    _capacity = new_capacity;
    char* new_data = new char[_capacity + 1]();
    
    if (_data != nullptr)
        std::memcpy(new_data, _data, _size);

    if (_data != nullptr)
        delete[] _data;
    _data = new_data;       
}

void String::ShrinkToFit() {
    _capacity = _size;
}

//------------------------operators----------------------------

char& String::operator[](const std::size_t idx) {
    return _data[idx];
}

const char& String::operator[](const std::size_t idx) const {
    return _data[idx];
}

String& String::operator=(const String& str) {
    if (*this == str)
        return *this;

    Reserve(str._capacity);
    _size = 0;

    for (std::size_t i = 0; i < str._size; ++i)
        PushBack(str[i]);

    ShrinkToFit();
    return *this;
}

String& String::operator+=(const String& str) {
    if (_size + str._size > _capacity)
        Reserve(std::max(_size, str._size) * 2);
    if (str._data != nullptr)
        memcpy(_data + _size, str._data, str._size);
    _size += str._size;
    return *this;
}

String String::operator+(const String& str) const {
    String result = *this;
    for (std::size_t i = 0; i < str._size; ++i)
        result.PushBack(str[i]);

    return result;
}

bool String::operator==(const String& str) const {
    if (_size != str._size)
        return false;

    for (std::size_t i = 0; i < _size; ++i)
        if (_data[i] != str[i])
            return false;

    return true;
}

int operator<=>(const String& str1, const String& str2) {
    for (size_t i = 0; i < std::min(str1.Size(), str2.Size()); ++i) {
        if (str1[i] > str2[i])
            return 1;
        else if (str1[i] < str2[i])
            return -1;
    }

    if (str1.Size() > str2.Size())
        return 1;
    else if (str1.Size() < str2.Size())
        return -1;

    return 0; 
}

std::ostream& operator<<(std::ostream& out, const String& str) {
    for (std::size_t i = 0; i < str.Size(); ++i)
        out << str[i];
    return out;
}

std::istream& operator>>(std::istream& in, String& str) {
    in.getline(const_cast<char*>(str.Data()), str.Size());
    return in;
}

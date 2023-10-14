#pragma once


class Any;

template<typename T>
T AnyCast(const Any& value);

class Any {

template<typename T>
friend T AnyCast(const Any& value);

public:

    Any() : ptr_(nullptr) {}

    template<typename U>
    Any(const U& obj) : ptr_(new Derived<U>(obj)) {}

    Any(const Any& other) : ptr_(other.ptr_->copy()) {}

    Any(Any&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ~Any() {
        delete ptr_;
    }

    template<typename U>
    Any& operator=(const U& obj) {
        delete ptr_;
        ptr_ = new Derived<U>(obj);
        return *this;
    }

    Any& operator=(const Any& other) {
        Any temp = other;
        Swap(temp);
        return *this;
    }

    Any& operator=(Any&& other) {
        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    void Reset() {
        delete ptr_;
        ptr_ = nullptr;
    }

    void Swap(Any& other) {
        std::swap(ptr_, other.ptr_);
    }

    bool HasValue() const {
        return ptr_;
    }

private:

    struct Base {
        virtual Base* copy() = 0;
        virtual const std::type_info& type_info() const = 0;
        virtual ~Base() = default;
    };

    template<typename T>
    struct Derived: public Base {
        T object;

        Derived(const T& obj) : object(obj) {}
        Base* copy() override {
            return new Derived<T>(object);
        }
        const std::type_info& type_info() const override {
            return typeid(object);
        }
    };

    Base* ptr_;

};

template<typename T, typename... Args>
Any MakeAny(Args&&... args) {
    return Any(new T(std::forward<Args>(args)...));
}

template<typename T>
T AnyCast(const Any& value) {
    if (value.ptr_ == nullptr || typeid(T) != value.ptr_->type_info())
        throw std::bad_cast();
    return dynamic_cast<Any::Derived<T>*>(value.ptr_)->object;
}

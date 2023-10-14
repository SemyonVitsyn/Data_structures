#pragma once


template <typename T>
class Function;

template<typename R, typename... Args>
class Function<R(Args...)> {

public:

    Function() : ptr_(nullptr) {}

    template<typename F>
    Function(const F f) : ptr_(new Derived<F>(f)) {}

    Function(const Function& other) {
        if (other.ptr_ == nullptr)
            ptr_ = nullptr;
        else   
            ptr_ = other.ptr_->copy(); 
    }

    Function(Function&& other) : ptr_(other.ptr_) {
        other.ptr_ = nullptr;
    }

    ~Function() {
        delete ptr_;
    }

    Function& operator=(const Function& other) {
        if (other.ptr_ == ptr_)
            return *this;

        Function temp = other;
        std::swap(temp.ptr_, ptr_);
        return *this;
    }

    template<typename F>
    Function& operator= (const F& f) {
        Function temp = Function(f);
        std::swap(temp.ptr_, ptr_);
        return *this;
    }

    Function& operator=(Function&& other) {
        if (other.ptr_ == ptr_) 
            return * this;

        delete ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        return *this;
    }

    R operator()(Args&&... args) const {
        if (ptr_ == nullptr)
            throw std::bad_function_call();
        else
            return ptr_->Invoke(args...);
    }

private:

    struct Base {
        virtual Base* copy() = 0;
        virtual R Invoke(Args&&... args) = 0;
        virtual ~Base() = default;
    };

    template<typename Func>
    struct Derived: public Base {
        Func f;

        Derived(const Func& foo) : f(foo) {}
        Base* copy() override {
            return new Derived<Func>(f);
        }
        R Invoke(Args&&... args) override {
            return f(std::forward<Args>(args)...);
        }
    };

    Base* ptr_;

};

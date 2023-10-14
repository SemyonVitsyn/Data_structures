#pragma once


template<typename T>
class UniquePtr {
    
public:
    
//-----------------------constructors-------------------------

    UniquePtr() : ptr_(nullptr) {}

    UniquePtr(T* ptr) : ptr_(ptr) {} 

    UniquePtr(UniquePtr&& uptr) noexcept : UniquePtr(uptr.Release()) {}

    template<typename other_T>
    UniquePtr(UniquePtr<other_T>&& uptr) : UniquePtr(uptr.Release()) {}

//------------------------destructor--------------------------

    ~UniquePtr() {
        delete ptr_;
    };

//------------------------operators---------------------------

    UniquePtr& operator=(UniquePtr&& uptr) noexcept {
        Reset(uptr.ptr_);
        uptr.ptr_ = nullptr;

        return *this;
    }

    template<typename other_T>
    UniquePtr& operator=(UniquePtr<other_T>&& uptr) {
        UniquePtr<T> temp(uptr.Release());
        temp.Swap(*this);

        return *this;
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    operator bool() const {
        return ptr_ != nullptr;
    }

//--------------------------methods---------------------------

    T* Get() const {
        return ptr_;
    }

    T* Release() noexcept {
        T* result = ptr_;
        ptr_ = nullptr;
        return result;
    }

    void Swap(UniquePtr& uptr) noexcept {
        std::swap(ptr_, uptr.ptr_);
    }

    void Reset(T* ptr = nullptr) noexcept {
        if (ptr_)
            delete ptr_;
        
        ptr_ = ptr;
    }

//------------------------------------------------------------

    UniquePtr(const UniquePtr& ptr) = delete;
    UniquePtr operator=(const UniquePtr& ptr) = delete;

private:
    T* ptr_;

};

template <typename T, typename... Args>
UniquePtr<T> MakeUnique(Args&&... args) {
    return UniquePtr<T>(new T(std::forward<Args>(args)...));
}

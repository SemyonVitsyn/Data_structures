#pragma once


struct ControlBlock {
    size_t shared_count;
    size_t weak_count;

    ControlBlock(size_t shr_count, size_t wk_count) 
        : shared_count(shr_count), weak_count(wk_count) {}

    ~ControlBlock() = default;
};

template<typename T>
class SharedPtr;

template<typename T>
class WeakPtr {

friend class SharedPtr<T>;

public:

//-----------------------constructors-------------------------

    WeakPtr() noexcept
        : ptr_(nullptr), cb(nullptr) {}

    WeakPtr(const WeakPtr& other) noexcept
        : ptr_(other.ptr_), cb(other.cb) 
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    WeakPtr(WeakPtr&& other) noexcept
        : ptr_(other.ptr_), cb(other.cb) 
    {
        other.ptr_ = nullptr;
        other.cb = nullptr;
    }

    WeakPtr(const SharedPtr<T>& sh_ptr) noexcept
        : ptr_(sh_ptr.ptr_), cb(sh_ptr.cb)
    {
        if (cb != nullptr)
            ++cb->weak_count;
    }

    template<typename U>
    WeakPtr<T>(WeakPtr<U>&& other) {
        if (std::is_convertible<U, T>()) 
            WeakPtr(static_cast<T&&>(other));
        else    
            WeakPtr();
    }

//------------------------destructor--------------------------

    ~WeakPtr() {
        if (cb == nullptr)
            return;

        --cb->weak_count;
        if (cb->shared_count == 0 && cb->weak_count == 0) {
            delete cb;
            cb = nullptr;
        }
    }

//------------------------operators---------------------------

    WeakPtr& operator=(const WeakPtr& other) noexcept {
        if (*this == other)
            return *this;

        if (cb != nullptr) {
            --cb->weak_count;
            if (cb->shared_count == 0 && cb->weak_count == 0) {
                delete cb;
                cb = nullptr;
            }
        }

        ptr_ = other.ptr_;
        cb = other.cb;

        if (cb != nullptr)
            ++cb->weak_count;

        return *this;
    }

    WeakPtr& operator=(WeakPtr&& other) noexcept {
        if (*this == other)
            return *this;

        if (cb != nullptr) {
            --cb->weak_count;
            if (cb->shared_count == 0 && cb->weak_count == 0) {
                delete cb;
                cb = nullptr;
            }
        }

        this->ptr_ = other.ptr_;
        this->cb = other.cb;

        other.ptr_ = nullptr;
        other.cb = nullptr;

        return *this;
    }

    template<typename U>
    WeakPtr<T>& operator=(WeakPtr<U>&& other) {
        return this->operator=(dynamic_cast<T&&>(other));
    }

    bool operator==(const WeakPtr& other) const {
        return (ptr_ == other.ptr_ && cb == other.cb);
    }

//-------------------------methods----------------------------

    void Swap(WeakPtr<T>& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(cb, other.cb);
    }

    void Reset() {
        if (cb != nullptr) {
            --cb->weak_count;
            if (cb->shared_count == 0 && cb->weak_count == 0)
                delete cb;
        }

        ptr_ = nullptr;
        cb = nullptr;
    }

    size_t UseCount() const {
        if (cb == nullptr)
            return 0;
        else    
            return cb->shared_count;
    }

    bool Expired() const {
        if (cb == nullptr)
            return true;
        else  
            return (cb->shared_count == 0);
    }

    SharedPtr<T> Lock() const {
        if (Expired())
            return SharedPtr<T>();
        else
            return SharedPtr<T>(*this);
    }

//------------------------------------------------------------

private:
    T* ptr_;
    ControlBlock* cb;

};


template <typename T>
class SharedPtr {

friend class WeakPtr<T>;

public:

//-----------------------constructors-------------------------

    SharedPtr() noexcept
        : ptr_(nullptr), cb(nullptr) {}

    SharedPtr(T* ptr) 
        : ptr_(ptr)
    {
        if (ptr_ != nullptr) 
            cb = new ControlBlock(1, 0);
        else
            cb = nullptr;
    }

    SharedPtr(const SharedPtr& other) noexcept
        : ptr_(other.ptr_), cb(other.cb) 
    {
        if (cb != nullptr)
            ++cb->shared_count;
    }

    SharedPtr(SharedPtr&& other) noexcept
        : ptr_(other.ptr_), cb(other.cb)
    {
        other.ptr_ = nullptr;
        other.cb = nullptr;
    }

    SharedPtr(const WeakPtr<T>& wk_ptr) 
        : ptr_(wk_ptr.ptr_), cb(wk_ptr.cb)
    {
        if (!wk_ptr.Expired()) {
            if (cb != nullptr)
                ++cb->shared_count;
        }
        else 
            throw std::bad_weak_ptr();
    }
    
    template<typename U>
    SharedPtr<T>(SharedPtr<U>&& other) {
        if (std::is_convertible<U, T>()) 
            SharedPtr<T>(dynamic_cast<T&&>(other));
        else    
            SharedPtr<T>();
    }

//------------------------destructor--------------------------

    ~SharedPtr() {
        if (cb == nullptr)
            return;

        --cb->shared_count;
        if (cb->shared_count == 0) {
            delete ptr_;
            ptr_ = nullptr;
            if (cb->weak_count == 0) {
                delete cb;
                cb = nullptr;
            }
        }
    }

//------------------------operators---------------------------

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (*this == other)
            return *this;

        if (cb != nullptr) {
            --cb->shared_count;
            if (cb->shared_count == 0) {
                delete ptr_;
                ptr_ = nullptr;
                if (cb->weak_count == 0) {
                    delete cb;
                    cb = nullptr;
                }
            }
        }

        ptr_ = other.ptr_;
        cb = other.cb;

        if (cb != nullptr)
            ++cb->shared_count;

        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (*this == other)
            return *this;

        if (cb != nullptr) {
            --cb->shared_count;
            if (cb->shared_count == 0) {
                delete ptr_;
                if (cb->weak_count == 0) 
                    delete cb;
            }
        }

        ptr_ = other.ptr_;
        cb = other.cb;

        other.ptr_ = nullptr;
        other.cb = nullptr;

        return *this;
    }

    template<typename U>
    SharedPtr<T>& operator=(SharedPtr<U>&& other) {
        return this->operator=(dynamic_cast<T&&>(other));
    }

    T& operator*() const {
        return *ptr_;
    }

    T* operator->() const {
        return ptr_;
    }

    operator bool() const {
        return (ptr_ != nullptr);
    }

    bool operator==(const SharedPtr& other) const {
        return (ptr_ == other.ptr_ && cb == other.cb);
    }
 
//-------------------------methods----------------------------

void Reset(T* ptr = nullptr) {
    if (ptr_ == ptr) 
        return;

    if (cb != nullptr) {
        --cb->shared_count;
        if (cb->shared_count == 0) {
            delete ptr_;
            ptr_ = nullptr;
            if (cb->weak_count == 0) {
                delete cb;
                cb = nullptr;
            }
        }       
    }

    ptr_ = ptr;
    if (ptr_ != nullptr)
        cb = new ControlBlock(1, 0);
    else
        cb = nullptr;  
}

void Swap(SharedPtr& other) {
    std::swap(ptr_, other.ptr_);
    std::swap(cb, other.cb);
}

T* Get() const {
    return ptr_;
}

size_t UseCount() const {
    if (cb != nullptr)
        return cb->shared_count;
    else
        return 0;
}

//------------------------------------------------------------

private:
    T* ptr_;
    ControlBlock* cb;

};

template<typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

#ifndef DEQUE_H
#define DEQUE_H
#include <cstddef>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include <deque>

template <typename T>
class Deque {
private:
    static const size_t kChunkSize = 128;

    template <typename U>
    class BaseIterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type = U;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

        BaseIterator() = default;
        BaseIterator(U** chunk, size_t idx = 0) : chunk_ptr(chunk), internal_idx(idx) {
        }

        BaseIterator& operator++() {
            if (internal_idx < kChunkSize - 1) {
                internal_idx++;
            } else {
                chunk_ptr++;
                internal_idx = 0;
            }

            return *this;
        }

        BaseIterator operator++(int) {
            auto copy = *this;
            ++(*this);
            return copy;
        }

        BaseIterator& operator--() {
            if (internal_idx > 0) {
                internal_idx--;
            } else {
                chunk_ptr--;
                internal_idx = kChunkSize - 1;
            }

            return *this;
        }

        BaseIterator operator--(int) {
            auto copy = *this;
            --(*this);
            return copy;
        }

        BaseIterator& operator+=(difference_type difference) {
            if (difference < 0) {
                return *this -= -difference;
            }

            size_t diff = static_cast<size_t>(difference);
            if (internal_idx + diff < kChunkSize) {
                internal_idx += diff;
            } else {
                diff += internal_idx;
                chunk_ptr += diff / kChunkSize;
                internal_idx = diff % kChunkSize;
            }

            return *this;
        }

        BaseIterator& operator-=(difference_type difference) {
            if (difference < 0) {
                return *this += -difference;
            }

            size_t diff = static_cast<size_t>(difference);
            if (internal_idx >= diff) {
                internal_idx -= diff;
            } else {
                diff += kChunkSize - internal_idx - 1;
                chunk_ptr -= diff / kChunkSize;
                internal_idx = kChunkSize - diff % kChunkSize - 1;
            }

            return *this;
        }

        BaseIterator operator+(difference_type diff) const {
            auto copy = *this;
            copy += diff;
            return copy;
        }

        BaseIterator operator-(difference_type diff) const {
            auto copy = *this;
            copy -= diff;
            return copy;
        }

        difference_type operator+(const BaseIterator& other) const {
            if (chunk_ptr == other.chunk_ptr) {
                return static_cast<difference_type>(internal_idx + other.internal_idx);
            }

            return static_cast<difference_type>(static_cast<size_t>(chunk_ptr + other.chunk_ptr) * kChunkSize +
                                                internal_idx + other.internal_idx);
        }

        difference_type operator-(const BaseIterator& other) const {
            if (chunk_ptr == other.chunk_ptr) {
                return static_cast<difference_type>(internal_idx - other.internal_idx);
            }

            return static_cast<difference_type>(static_cast<size_t>(chunk_ptr - other.chunk_ptr) * kChunkSize +
                                                internal_idx - other.internal_idx);
        }

        bool operator<(const BaseIterator& other) const {
            return (chunk_ptr < other.chunk_ptr || (chunk_ptr == other.chunk_ptr && internal_idx < other.internal_idx));
        }

        bool operator>(const BaseIterator& other) const {
            return (chunk_ptr > other.chunk_ptr || (chunk_ptr == other.chunk_ptr && internal_idx > other.internal_idx));
        }

        bool operator<=(const BaseIterator& other) const {
            return !(*this > other);
        }

        bool operator>=(const BaseIterator& other) const {
            return !(*this < other);
        }

        bool operator==(const BaseIterator& other) const {
            return chunk_ptr == other.chunk_ptr && internal_idx == other.internal_idx;
        }

        bool operator!=(const BaseIterator& other) const {
            return !(*this == other);
        }

        U& operator*() {
            return *GetObjectPtr();
        }

        U* operator->() {
            return GetObjectPtr();
        }

        friend BaseIterator operator+(const difference_type& diff, const BaseIterator& it) {
            return it + diff;
        }

        operator BaseIterator<const U>() const {
            return BaseIterator<const U>(const_cast<const T**>(chunk_ptr), internal_idx);
        }

    private:
        U* GetObjectPtr() {
            return *chunk_ptr + internal_idx;
        }

    private:
        U** chunk_ptr = nullptr;
        size_t internal_idx = 0;
    };

public:
    using value_type = T;
    using iterator = BaseIterator<T>;
    using const_iterator = BaseIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    Deque() : chunks_count_(1), chunks_(CreateChunks(chunks_count_)) {
        AllocateChunks(chunks_, 0, chunks_count_);
        begin_ = end_ = DataBeginIterator() + kChunkSize / 2;
    }

    explicit Deque(size_t size)
        : chunks_count_(((size + 1) + 1) / kChunkSize + 1), chunks_(CreateChunks(chunks_count_)) {
        ConstructObjects(size);
    }

    Deque(size_t size, const T& item)
        : chunks_count_(((size + 1) + 1) / kChunkSize + 1), chunks_(CreateChunks(chunks_count_)) {
        ConstructObjects(size, item);
    }

    Deque(const std::initializer_list<T> &list) : Deque() {
        for (const auto& elem: list) {
            push_back(elem);
        }
    }

    ~Deque() {
        DestroyObjects(begin_, end_);
        DeallocateChunks(chunks_, 0, chunks_count_);
        DestroyChunks(chunks_);
    }

    Deque(const Deque& other) : chunks_count_(other.chunks_count_), chunks_(CreateChunks(chunks_count_)) {
        AllocateChunks(chunks_, 0, chunks_count_);
        begin_ = DataBeginIterator() + static_cast<ptrdiff_t>(other.begin_ - other.DataBeginIterator());
        end_ = DataBeginIterator() + static_cast<ptrdiff_t>(other.end_ - other.DataBeginIterator());

        for (auto it = begin_, other_it = other.begin_; it != end_; ++it, ++other_it) {
            try {
                new (&(*it)) T(*other_it);
            } catch (...) {
                DestroyObjects(begin_, it);
                DeallocateChunks(chunks_, 0, chunks_count_);
                DestroyChunks(chunks_);
                throw;
            }
        }
    }

    Deque& operator=(const Deque& other) {
        if (this != &other) {
            Deque copy(other);
            std::swap(chunks_count_, copy.chunks_count_);
            std::swap(chunks_, copy.chunks_);
            std::swap(begin_, copy.begin_);
            std::swap(end_, copy.end_);
        }

        return *this;
    }

    bool operator==(const Deque& other) const {
        if (size() == other.size()) {
            for (auto it1 = begin_, it2 = other.begin_; it1 != end_; ++it1, ++it2) {
                if (*it1 != *it2) {
                    return false;
                } 
            }

            return true;
        }

        return false;
    }

        bool operator==(const std::deque<T>& other) const {
        if (size() == other.size()) {
            auto it2 = other.begin();
            for (auto it1 = begin_; it1 != end_; ++it1, ++it2) {
                if (*it1 != *it2) {
                    return false;
                } 
            }

            return true;
        }

        return false;
    }

    T& front() {
        return *begin_;
    }

    const T& front() const {
        return *begin_;
    }

    T& back() {
        return *(end_ - 1);
    }

    const T& back() const {
        return *(end_ - 1);
    }

    size_t size() const {
        return static_cast<size_t>(end_ - begin_);
    }

    bool empty() const {
        return size() == 0;
    }

    T& operator[](size_t idx) {
        return *(begin_ + static_cast<ptrdiff_t>(idx));
    }

    const T& operator[](size_t idx) const {
        return *(begin_ + static_cast<ptrdiff_t>(idx));
    }

    T& at(size_t idx) {
        if (idx >= size()) {
            throw std::out_of_range("");
        }

        return *(begin_ + idx);
    }

    const T& at(size_t idx) const {
        if (idx >= size()) {
            throw std::out_of_range("");
        }
        return *(begin_ + idx);
    }

    void push_back(const T& item) {
        if (end_ + 1 == iterator(chunks_ + chunks_count_ - 1, kChunkSize - 1)) {
            IncreaseAndInsert(end_, item);
        } else {
            new (&(*end_)) T(item);
        }

        end_++;
    }

    void push_front(const T& item) {
        if (begin_ - 1 == DataBeginIterator()) {
            IncreaseAndInsert(begin_ - 1, item);
        } else {
            new (&(*(begin_ - 1))) T(item);
        }

        begin_--;
    }

    void pop_back() {
        (end_ - 1)->~T();
        end_--;
    }

    void pop_front() {
        begin_->~T();
        begin_++;
    }

    iterator insert(iterator insert_it, const T& item) {
        ptrdiff_t shift = insert_it - begin_;
        push_back(item);
        insert_it = begin_ + shift;
        for (auto it = end_ - 1; it != insert_it; --it) {
            std::swap(*it, *(it - 1));
        }

        return insert_it;
    }

    iterator erase(iterator erase_it) {
        erase_it->~T();
        for (auto it = erase_it + 1; it != end_; ++it) {
            new (&(*(it - 1))) T(*it);
            it->~T();
        }
        end_--;

        return erase_it;
    }

    iterator begin() {
        return begin_;
    }

    const_iterator begin() const {
        return begin_;
    }

    iterator end() {
        return end_;
    }

    const_iterator end() const {
        return end_;
    }

    const_iterator cbegin() const {
        return begin_;
    }

    const_iterator cend() const {
        return end_;
    }

    reverse_iterator rbegin() {
        return std::reverse_iterator(end());
    }

    const_reverse_iterator rbegin() const {
        return std::reverse_iterator(end());
    }

    reverse_iterator rend() {
        return std::reverse_iterator(begin());
    }

    const_reverse_iterator rend() const {
        return std::reverse_iterator(begin());
    }

    const_reverse_iterator crbegin() const {
        return rbegin();
    }

    const_reverse_iterator crend() const {
        return rend();
    }

private:
    T** CreateChunks(size_t chunks_count) {
        return new T*[chunks_count];
    }

    void AllocateChunks(T** chunks, size_t begin, size_t end) {
        for (size_t i = begin; i < end; ++i) {
            try {
                chunks[i] = reinterpret_cast<T*>(new char[sizeof(T) * kChunkSize]);
            } catch (...) {
                DeallocateChunks(chunks, begin, i);
                DestroyChunks(chunks);
                throw;
            }
        }
    }

    template <typename... Args>
    void ConstructObjects(size_t size, const Args&... args) {
        AllocateChunks(chunks_, 0, chunks_count_);
        begin_ = DataBeginIterator() + 1;
        end_ = begin_ + static_cast<ptrdiff_t>(size);

        for (iterator it = begin_; it != end_; ++it) {
            try {
                new (&(*it)) T(args...);
            } catch (...) {
                DestroyObjects(begin_, it);
                DeallocateChunks(chunks_, 0, chunks_count_);
                DestroyChunks(chunks_);
                throw;
            }
        }
    }

    void DestroyChunks(T** chunks) {
        delete[] chunks;
    }

    void DeallocateChunks(T** chunks, size_t begin, size_t end) {
        for (size_t i = begin; i < end; ++i) {
            delete[] reinterpret_cast<char*>(chunks[i]);
        }
    }

    void DestroyObjects(iterator begin, iterator end) {
        for (auto it = begin; it != end; ++it) {
            it->~T();
        }
    }

    iterator DataBeginIterator() const {
        return iterator(chunks_, 0);
    }

    void IncreaseAndInsert(iterator it, const T& item) {
        ptrdiff_t shift = it - begin_;

        size_t unused_chunks_left = static_cast<size_t>(begin_ - DataBeginIterator()) / kChunkSize;
        size_t unused_chunks_right =
            chunks_count_ - static_cast<size_t>(end_ - DataBeginIterator() - 1) / kChunkSize - 1;

        size_t add_chunks_count = chunks_count_;
        if ((add_chunks_count + unused_chunks_right - unused_chunks_left) % 2) {
            ++add_chunks_count;
        }

        size_t add_chunks_left = (add_chunks_count + unused_chunks_right - unused_chunks_left) / 2;

        size_t new_chunks_count = chunks_count_ + add_chunks_count;

        T** new_chunks = CreateChunks(new_chunks_count);
        AllocateChunks(new_chunks, 0, add_chunks_left);
        try {
            AllocateChunks(new_chunks, add_chunks_left + chunks_count_, new_chunks_count);
        } catch (...) {
            DeallocateChunks(new_chunks, 0, add_chunks_left);
            throw;
        }

        try {
            std::copy(chunks_, chunks_ + chunks_count_, new_chunks + add_chunks_left);
        } catch (...) {
            DeallocateChunks(new_chunks, 0, add_chunks_left);
            DeallocateChunks(new_chunks, add_chunks_left + chunks_count_, new_chunks_count);
            throw;
        }

        it = iterator(new_chunks, 0) +
             static_cast<ptrdiff_t>(begin_ - DataBeginIterator() +
                                    static_cast<ptrdiff_t>(kChunkSize * add_chunks_left)) +
             shift;
        try {
            new (&(*it)) T(item);
        } catch (...) {
            DeallocateChunks(new_chunks, 0, add_chunks_left);
            DeallocateChunks(new_chunks, add_chunks_left + chunks_count_, new_chunks_count);
            throw;
        }

        DestroyChunks(chunks_);

        ptrdiff_t begin_shift = begin_ - DataBeginIterator() + static_cast<ptrdiff_t>(kChunkSize * add_chunks_left);
        ptrdiff_t end_shift = end_ - DataBeginIterator() + static_cast<ptrdiff_t>(kChunkSize * add_chunks_left);

        chunks_ = new_chunks;
        chunks_count_ = new_chunks_count;

        begin_ = DataBeginIterator() + begin_shift;
        end_ = DataBeginIterator() + end_shift;
    }

private:
    size_t chunks_count_ = 0;
    T** chunks_ = nullptr;
    iterator begin_ = iterator();
    iterator end_ = iterator();
};

#endif

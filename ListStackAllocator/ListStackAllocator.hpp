#pragma once

#include <exception>
#include <memory>
#include <stdexcept>

template <size_t N>
class StackStorage {
private:
    char buffer_[N];
    char* used_;

public:
    StackStorage() : buffer_(), used_(buffer_) {}

    StackStorage(const StackStorage&) = delete;
    StackStorage& operator=(const StackStorage&) = delete; 

    char* allocate(size_t bytes, size_t alignment) {
        size_t shift = reinterpret_cast<size_t>(used_) % alignment;
        if (shift > 0) {
            used_ += alignment - shift;
        }
        char* ptr = used_;
        used_ += bytes;
        return ptr;
    }
};

template<typename T, size_t N>
class StackAllocator {
public:
    using value_type = T;
    using propagate_on_container_copy_assignment = std::false_type;

    StackStorage<N>* storage_;

    explicit StackAllocator(const StackStorage<N>& storage) : storage_(const_cast<StackStorage<N>*>(&storage)) {}

    template<typename U>
    StackAllocator(const StackAllocator<U, N>& other) : storage_(other.storage_) {}

    T* allocate(size_t bytes) {
        return reinterpret_cast<T*>(storage_->allocate(bytes * sizeof(T), alignof(T)));
    }

    void deallocate(T*, size_t) {}

    template <typename U>
    struct rebind {
        using other = StackAllocator<U, N>;
    };
};

template <typename T, typename Allocator = std::allocator<T>>
class List {
private:
    struct BaseNode {
        BaseNode* next = nullptr;
        BaseNode* prev = nullptr;
    };

    struct Node : BaseNode {
        T item;

        Node() = default;
        Node(const Node&) = default;
        explicit Node(const T& val) : item(val) {}
    };

    template <typename U>
    class BaseIterator {
    public:
        using iterator_category = std::bidirectional_iterator_tag;
        using value_type = U;
        using difference_type = ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;

    
        BaseIterator() = default;
        explicit BaseIterator(BaseNode* node) : node_(node) {}

        BaseIterator& operator++() {
            node_ = node_->next;
            return *this;
        }

        BaseIterator operator++(int) {
            auto it = *this;
            ++(*this);
            return it;
        }

        BaseIterator& operator--() {
            node_ = node_->prev;
            return *this;
        }

        BaseIterator operator--(int) {
            auto it = *this;
            --(*this);
            return it;
        }

        U& operator*() const {
            return static_cast<Node*>(node_)->item;
        }

        U* operator->() const {
            return &operator*();
        }

        bool operator==(const BaseIterator& other) const {
            return node_ == other.node_;
        }

        bool operator!=(const BaseIterator& other) const {
            return !(*this == other);
        }

        operator BaseIterator<const U>() const {
            return BaseIterator<const U>(node_);
        }
        
        BaseNode* getNode() {
            return node_;
        }

    private:
        BaseNode* node_ = nullptr;
    };

public:
    using value_type = T;
    using iterator = BaseIterator<T>;
    using const_iterator = BaseIterator<const T>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    using NodeAllocator = typename std::allocator_traits<Allocator>::template rebind_alloc<Node>;
    using NodeTraits = std::allocator_traits<NodeAllocator>;

    List() : List(Allocator()) {}

    explicit List(size_t size) : List(size, Allocator()) {}

    List(size_t size, const T& val) : List(size, val, Allocator()) {}

    explicit List(const Allocator& allocator) : allocator_(allocator), node_allocator_(allocator) {
        fake_node_.next = fake_node_.prev = &fake_node_;
    };

    List(size_t size, const Allocator& allocator) : List(allocator) {
        try {
            for (size_t i = 0; i < size; ++i) {
                Node* node = CreateNode();
                PushBackNode(node);
            }
        } catch (...) {
            Destroy();
            throw;
        }
    }

    List(size_t size, const T& val, const Allocator& allocator) : List(allocator) {
        try {
            for (size_t i = 0; i < size; ++i) {
                Node* node = CreateNode(val);
                PushBackNode(node);
            }
        } catch (...) {
            Destroy();
            throw;
        }
    }

    ~List() {
        Destroy();
    }

    List(const List& other) : List(std::allocator_traits<Allocator>::select_on_container_copy_construction(other.allocator_)) {
        try {
            for (const T& val : other) {
                PushBackNode(CreateNode(val));
            }
        } catch (...) {
            Destroy();
            throw;
        }
    }

    List(const List& other, const Allocator& allocator) : List(allocator) {
        try {
            for (const T& val : other) {
                PushBackNode(CreateNode(val));
            }
        } catch (...) {
            Destroy();
            throw;
        }
    }

    List& operator=(const List& other) {
        if (this != &other) {
            List copy(other, std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value ? other.allocator_ : allocator_);
            Swap(copy);
        }
        return *this;
    }

    Allocator get_allocator() const {
        return allocator_;
    }

    size_t size() const {
        return size_;
    }

    void push_back(const T& item) {
        PushBackNode(CreateNode(item));
    }

    void push_front(const T& item) {
        PushFrontNode(CreateNode(item));
    }

    void pop_back() {
        DestroyNode(PopBackNode());
    }

    void pop_front() {
        DestroyNode(PopFrontNode());
    }

    iterator insert(const_iterator it, const T& item) {
        Node* node = CreateNode(item);

        node->next = it.getNode();
        node->prev = node->next->prev;

        node->next->prev = node->prev->next = node;
        size_++;
        return iterator(node);
    }

    iterator erase(const_iterator it) {
        BaseNode* node = it.getNode();

        node->next->prev = node->prev;
        node->prev->next = node->next;

        iterator next_iter = iterator(node->next);
        DestroyNode(static_cast<Node*>(node));
        size_--;
        return next_iter;
    }


    iterator begin() {
        return iterator(fake_node_.next);
    }

    const_iterator begin() const {
        return iterator(fake_node_.next);
    }

    iterator end() {
        return iterator(&fake_node_);
    }

    const_iterator end() const {
        return iterator(const_cast<List::BaseNode*>(&fake_node_));
    }

    const_iterator cbegin() const {
        return begin();
    }

    const_iterator cend() const {
        return end();
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
    template <typename... Args>
    Node* CreateNode(const Args&... args) {
        Node* node = NodeTraits::allocate(node_allocator_, 1);
        try {
            NodeTraits::construct(node_allocator_, node, args...);
        } catch (...) {
            NodeTraits::deallocate(node_allocator_, node, 1);
            throw;
        }
        return node;
    }

    void PushBackNode(Node* node) {
        node->next = &fake_node_;
        node->prev = fake_node_.prev;

        fake_node_.prev->next = node;
        fake_node_.prev = node;
        size_++;
    }

    void PushFrontNode(Node* node) {
        node->next = fake_node_.next;
        node->prev = &fake_node_;

        fake_node_.next->prev = node;
        fake_node_.next = node;
        size_++;
    }

    Node* PopBackNode() {
        Node* node = static_cast<Node*>(fake_node_.prev);
        fake_node_.prev = node->prev;
        fake_node_.prev->next = &fake_node_;

        node->prev = node->next = nullptr;
        size_--;
        return node;
    }

    Node* PopFrontNode() {
        Node* node = static_cast<Node*>(fake_node_.next);
        fake_node_.next = node->next;
        fake_node_.next->prev = &fake_node_;

        node->prev = node->next = nullptr;
        size_--;
        return node;
    }

    void Swap(List& other) {
        fake_node_.next->prev = fake_node_.prev->next = &other.fake_node_;
        other.fake_node_.next->prev = other.fake_node_.prev->next = &fake_node_;

        std::swap(allocator_, other.allocator_);
        std::swap(node_allocator_, other.node_allocator_);
        std::swap(fake_node_, other.fake_node_);
        std::swap(size_, other.size_);
    }

    void DestroyNode(Node* node) {
        NodeTraits::destroy(node_allocator_, node);
        NodeTraits::deallocate(node_allocator_, node, 1);
    }

    void Destroy() {
        while (size_) {
            DestroyNode(PopBackNode());
        }
    }


private:
    Allocator allocator_;
    NodeAllocator node_allocator_;
    BaseNode fake_node_;
    size_t size_ = 0;
};

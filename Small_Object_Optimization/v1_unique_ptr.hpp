/**
 * @file    v1_unique_ptr.hpp
 * @brief   version 1 of SmallPtr: mimic unique_ptr behavior and interface
 */

#ifndef SMALL_OBJECT_OPTIMIZATION_V1_UNIQUE_PTR_HPP_
#define SMALL_OBJECT_OPTIMIZATION_V1_UNIQUE_PTR_HPP_

#include <type_traits>
#include <utility>

template <class T>
class SmallPtr
{
private:
    static_assert(!std::is_array<T>::value, "arrays not supported");
    T* m_ptr;

public:
    SmallPtr() noexcept : m_ptr(nullptr) {}
    explicit SmallPtr(T* ptr) noexcept : m_ptr(ptr) {}
    ~SmallPtr() { reset(); }

    SmallPtr(SmallPtr&& rhs) noexcept : m_ptr(nullptr) { this->swap(rhs); }
    SmallPtr& operator=(SmallPtr&& rhs) noexcept
    {
        this->swap(rhs);
        return *this;
    }
    SmallPtr& operator=(nullptr_t) noexcept
    {
        reset();
        return *this;
    }
    // disable copying
    SmallPtr(const SmallPtr&) = delete;
    SmallPtr& operator=(const SmallPtr&) = delete;

    void swap(SmallPtr& rhs) noexcept { std::swap(m_ptr, rhs.m_ptr); }

    void reset(T* ptr = nullptr) noexcept
    {
        if (m_ptr)
            delete m_ptr;
        m_ptr = ptr;
    }

    // access functions/operator
    T* get() noexcept { return m_ptr; }
    const T* get() const noexcept { return m_ptr; }

    T* operator->() noexcept { return get(); }
    const T* operator->() const noexcept { return get(); }

    T& operator*() noexcept { return *get(); }
    const T& operator*() const noexcept { return *get(); }

    // check functions
    bool operator==(nullptr_t) const noexcept { return get() == nullptr; }
    bool operator!=(nullptr_t) const noexcept { return get() != nullptr; }
    explicit operator bool() const noexcept { return *this != nullptr; }

    // TODO
    bool usesHeap() const noexcept { return true; }
    bool usesStack() const noexcept { return false; }
};

// generic swap specialization
template <typename T>
void swap(SmallPtr<T>& lhs, SmallPtr<T>& rhs)
{
    lhs.swap(rhs);
}

#endif /* SMALL_OBJECT_OPTIMIZATION_V1_UNIQUE_PTR_HPP_ */

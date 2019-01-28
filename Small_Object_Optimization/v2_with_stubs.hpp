/**
 * @file    v2_with_stubs.cpp
 * @brief   version 2 of SmallPtr: add dummies to define the final interface
 */

#ifndef SMALL_OBJECT_OPTIMIZATION_V2_WITH_STUBS_HPP_
#define SMALL_OBJECT_OPTIMIZATION_V2_WITH_STUBS_HPP_

#include <type_traits>
#include <utility>

/// dummy class that only transports type information
template <class T>
class InPlace
{
};

template <class T, size_t T_StackSize = 0>
class SmallPtr
{
private:
    static_assert(!std::is_array<T>::value, "arrays not supported");
    T* m_ptr;

public:
    SmallPtr() noexcept : m_ptr(nullptr) {}
    explicit SmallPtr(T* ptr) noexcept : m_ptr(ptr) {}
    ~SmallPtr() { reset(); }

    template <class Derived, class... Args>
    explicit SmallPtr(InPlace<Derived>, Args&&... args) : m_ptr(nullptr)
    {
        emplace<Derived>(std::forward<Args>(args)...);
    }

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

    template <class Derived, class... Args>
    void emplace(Args&&... args)
    {
        static_assert(std::is_same<T, Derived>::value or std::is_base_of<T, Derived>::value,
                      "may only use sub-classes of T!");
        static_assert(std::is_constructible<Derived, Args...>::value, "cannot instantiate!");

        reset(new Derived(std::forward<Args>(args)...));
    }
};

// generic swap specialization
template <class T, size_t N>
void swap(SmallPtr<T, N>& lhs, SmallPtr<T, N>& rhs)
{
    lhs.swap(rhs);
}

#endif /* SMALL_OBJECT_OPTIMIZATION_V2_WITH_STUBS_HPP_ */

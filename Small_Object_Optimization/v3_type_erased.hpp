/**
 * @file    v3_type_erased.cpp
 * @brief   version 3 of SmallPtr: use type erasure
 */

#pragma once

#include <type_traits>
#include <utility>

template <class T>
class IStorage
{
public:
    virtual ~IStorage() = default;
    virtual T* get() noexcept = 0;
    virtual const T* get() const noexcept = 0;
};

template <class Derived, class Base>
class Storage final : public IStorage<Base>
{
public:
    template <typename... Args>
    Storage(Args&&... args) : m_instance(std::forward<Args>(args)...)
    {
    }
    virtual ~Storage() = default;

    Base* get() noexcept final { return &m_instance; }
    const Base* get() const noexcept final { return &m_instance; }

private:
    Derived m_instance;
};

template <class Base>
class StoragePtr final : public IStorage<Base>
{
public:
    StoragePtr(Base* ptr) : m_instance(ptr) {}
    virtual ~StoragePtr() { delete m_instance; }

    Base* get() noexcept final { return m_instance; }
    const Base* get() const noexcept final { return m_instance; }

private:
    Base* m_instance;
};

/// dummy class that only transports type information
template <class T>
class InPlace
{
};

template <class T, size_t T_StackSize = 0> // TODO
class SmallPtr
{
private:
    static_assert(!std::is_array<T>::value, "arrays not supported");

    IStorage<T>* m_ptr;

    T* getPtr() noexcept
    {
        if (!m_ptr)
            return nullptr;
        return m_ptr->get();
    }
    const T* getPtr() const noexcept
    {
        if (!m_ptr)
            return nullptr;
        return m_ptr->get();
    }

public:
    SmallPtr() noexcept : m_ptr(nullptr) {}
    explicit SmallPtr(T* ptr) noexcept : m_ptr(ptr) { reset(ptr); }
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
    SmallPtr& operator=(std::nullptr_t) noexcept
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
        {
            delete m_ptr;
            m_ptr = nullptr;
        }
        if (ptr)
        {
            m_ptr = new StoragePtr<T>(ptr);
        }
    }

    // access functions/operator
    T* get() noexcept { return getPtr(); }
    const T* get() const noexcept { return getPtr(); }

    T* operator->() noexcept { return get(); }
    const T* operator->() const noexcept { return get(); }

    T& operator*() noexcept { return *get(); }
    const T& operator*() const noexcept { return *get(); }

    // check functions
    bool operator==(std::nullptr_t) const noexcept { return get() == nullptr; }
    bool operator!=(std::nullptr_t) const noexcept { return get() != nullptr; }
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

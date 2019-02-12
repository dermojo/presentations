/**
 * @file    v5_small_opt2.cpp
 * @brief   version 5 of SmallPtr: make it smaller
 */

#pragma once

#include <type_traits>
#include <utility>

template <class T>
class IStorage
{
public:
    virtual ~IStorage() = default;
    virtual void moveTo(void* stack, IStorage<T>*& toPtr, IStorage<T>*& fromPtr) = 0;
    virtual T* get() noexcept = 0;
    virtual const T* get() const noexcept = 0;
    virtual bool usesHeap() const noexcept = 0;
};

template <class Derived, class Base>
class StackStorage final : public IStorage<Base>
{
public:
    template <typename... Args>
    StackStorage(Args&&... args) : m_instance(std::forward<Args>(args)...)
    {
    }
    virtual ~StackStorage() = default;

    void moveTo(void* stack, IStorage<Base>*& toPtr, IStorage<Base>*&) final
    {
        toPtr = ::new (stack) StackStorage<Derived, Base>(std::move(m_instance));
    }

    Base* get() noexcept final { return &m_instance; }
    const Base* get() const noexcept final { return &m_instance; }

    bool usesHeap() const noexcept { return false; }

private:
    Derived m_instance;
};

template <class Derived, class Base>
class HeapStorage final : public IStorage<Base>
{
public:
    template <typename... Args>
    HeapStorage(Args&&... args) : m_instance(std::forward<Args>(args)...)
    {
    }
    virtual ~HeapStorage() = default;

    void moveTo(void*, IStorage<Base>*& toPtr, IStorage<Base>*& fromPtr) final
    {
        toPtr = fromPtr;
        fromPtr = nullptr;
    }

    Base* get() noexcept final { return &m_instance; }
    const Base* get() const noexcept final { return &m_instance; }

    bool usesHeap() const noexcept { return true; }

private:
    Derived m_instance;
};

template <class Base, bool T_OnHeap>
class StoragePtr final : public IStorage<Base>
{
public:
    StoragePtr(Base* ptr) : m_instance(ptr) {}
    virtual ~StoragePtr()
    {
        if (m_instance)
            delete m_instance;
    }

    void moveTo(void* stack, IStorage<Base>*& toPtr, IStorage<Base>*& fromPtr) final
    {
        if (T_OnHeap)
        {
            toPtr = fromPtr;
            fromPtr = nullptr;
        }
        else
        {
            toPtr = ::new (stack) StoragePtr(m_instance);
            m_instance = nullptr;
        }
    }

    Base* get() noexcept final { return m_instance; }
    const Base* get() const noexcept final { return m_instance; }

    bool usesHeap() const noexcept { return T_OnHeap; }

private:
    Base* m_instance;
};

/// dummy class that only transports type information
template <class T>
class InPlace
{
};

template <class T, size_t T_StackSize = 64>
class SmallPtr
{
private:
    static_assert(!std::is_array<T>::value, "arrays not supported");
    static constexpr std::size_t alignment = alignof(void*);

    IStorage<T>* m_ptr;
    typename std::aligned_storage<T_StackSize, alignment>::type m_stack;

    T* getPtr() noexcept
    {
        if (m_ptr)
            return m_ptr->get();
        return nullptr;
    }
    const T* getPtr() const noexcept
    {
        if (m_ptr)
            return m_ptr->get();
        return nullptr;
    }

public:
    SmallPtr() noexcept : m_ptr(nullptr) {}
    explicit SmallPtr(T* ptr) noexcept : m_ptr(nullptr) { reset(ptr); }
    ~SmallPtr() { reset(); }

    template <class Derived, class... Args>
    explicit SmallPtr(InPlace<Derived>, Args&&... args) : m_ptr(nullptr)
    {
        emplace<Derived>(std::forward<Args>(args)...);
    }

    SmallPtr(SmallPtr<T, T_StackSize>&& rhs) /* noexcept */ { assign(rhs); }
    SmallPtr& operator=(SmallPtr<T, T_StackSize>&& rhs) /* noexcept */
    {
        assign(rhs);
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

    // void swap(SmallPtr& rhs) noexcept { TODO; }

    void reset(T* ptr = nullptr) noexcept
    {
        if (m_ptr)
        {
            if (m_ptr->usesHeap())
                delete m_ptr;
            else
                m_ptr->~IStorage();

            m_ptr = nullptr;
        }
        if (ptr)
        {
            // support the fact that m_stack may be too small for a pointer...
            using StackType = StoragePtr<T, false>;
            using HeapType = StoragePtr<T, true>;
            emplaceImpl<StackType, HeapType>(ptr);
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

    bool usesHeap() const noexcept { return m_ptr && m_ptr->usesHeap(); }
    bool usesStack() const noexcept { return !usesHeap(); }

    template <class Derived, class... Args>
    void emplace(Args&&... args)
    {
        static_assert(std::is_same<T, Derived>::value or std::is_base_of<T, Derived>::value,
                      "may only use sub-classes of T!");
        static_assert(std::is_constructible<Derived, Args...>::value, "cannot instantiate!");

        reset();
        using StackType = StackStorage<Derived, T>;
        using HeapType = HeapStorage<Derived, T>;
        emplaceImpl<StackType, HeapType>(std::forward<Args>(args)...);
    }

private:
    template <class StackType, class HeapType, class... Args>
    void emplaceImpl(Args&&... args)
    {
        if (sizeof(StackType) <= T_StackSize)
        {
            // small type
            void* buffer = &m_stack;
            m_ptr = ::new (buffer) StackType(std::forward<Args>(args)...);
        }
        else
        {
            // large type
            m_ptr = new HeapType(std::forward<Args>(args)...);
        }
    }

    void assign(SmallPtr<T, T_StackSize>& rhs) /* noexcept */
    {
        if (rhs.m_ptr)
        {
            rhs.m_ptr->moveTo(&m_stack, m_ptr, rhs.m_ptr);
        }
        rhs.reset();
    }
};

// generic swap specialization
// template <class T, size_t N>
// void swap(SmallPtr<T, N>& lhs, SmallPtr<T, N>& rhs)
//{
//    lhs.swap(rhs);
//}

using std::swap;

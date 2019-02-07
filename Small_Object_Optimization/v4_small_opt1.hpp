/**
 * @file    v4_small_opt1.cpp
 * @brief   version 4 of SmallPtr: add stack space + placement new
 */

#pragma once

#include <type_traits>
#include <utility>

template <class T>
class IStorage
{
public:
    virtual ~IStorage() = default;
    virtual IStorage<T>* moveTo(void*) = 0;
    virtual T* get() noexcept = 0;
    virtual const T* get() const noexcept = 0;
};

template <class Derived, class Base>
class Storage : public IStorage<Base>
{
public:
    template <typename... Args>
    Storage(Args&&... args) : m_instance(std::forward<Args>(args)...)
    {
    }
    virtual ~Storage() = default;

    IStorage<Base>* moveTo(void* stack) final
    {
        return ::new (stack) Storage<Derived, Base>(std::move(m_instance));
    }

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
    virtual ~StoragePtr()
    {
        if (m_instance)
            delete m_instance;
    }

    IStorage<Base>* moveTo(void* stack) final
    {
        IStorage<Base>* ptr = ::new (stack) StoragePtr(m_instance);
        m_instance = nullptr;
        return ptr;
    }

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

template <class T, size_t T_StackSize = 64>
class SmallPtr
{
private:
    static_assert(!std::is_array<T>::value, "arrays not supported");
    static constexpr std::size_t alignment = alignof(void*);

    IStorage<T>* m_ptr;
    typename std::aligned_storage<T_StackSize, alignment>::type m_stack;
    bool m_usesHeap;

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
    SmallPtr() noexcept : m_ptr(nullptr), m_usesHeap(false) {}
    explicit SmallPtr(T* ptr) noexcept : m_ptr(nullptr), m_usesHeap(false) { reset(ptr); }
    ~SmallPtr() { reset(); }

    template <class Derived, class... Args>
    explicit SmallPtr(InPlace<Derived>, Args&&... args) : m_ptr(nullptr), m_usesHeap(false)
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
            if (m_usesHeap)
                delete m_ptr;
            else
                m_ptr->~IStorage();

            m_ptr = nullptr;
            m_usesHeap = false;
        }
        if (ptr)
        {
            emplaceImpl<StoragePtr<T>>(ptr);
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

    bool usesHeap() const noexcept { return m_usesHeap; }
    bool usesStack() const noexcept { return !usesHeap(); }

    template <class Derived, class... Args>
    void emplace(Args&&... args)
    {
        static_assert(std::is_same<T, Derived>::value or std::is_base_of<T, Derived>::value,
                      "may only use sub-classes of T!");
        static_assert(std::is_constructible<Derived, Args...>::value, "cannot instantiate!");

        reset();
        using ConstructedType = Storage<Derived, T>;
        emplaceImpl<ConstructedType>(std::forward<Args>(args)...);
    }

private:
    template <class ConstructedType, class... Args>
    void emplaceImpl(Args&&... args)
    {
        if (sizeof(ConstructedType) <= T_StackSize)
        {
            // small type
            void* buffer = &m_stack;
            m_ptr = ::new (buffer) ConstructedType(std::forward<Args>(args)...);
            m_usesHeap = false;
        }
        else
        {
            // large type
            m_ptr = new ConstructedType(std::forward<Args>(args)...);
            m_usesHeap = true;
        }
    }

    void assign(SmallPtr<T, T_StackSize>& rhs) /* noexcept */
    {
        if (!rhs.m_usesHeap && rhs.m_ptr)
        {
            m_ptr = rhs.m_ptr->moveTo(&m_stack);
        }
        else
        {
            m_ptr = rhs.m_ptr;
        }
        m_usesHeap = rhs.m_usesHeap;

        rhs.m_ptr = nullptr;
        rhs.m_usesHeap = false;
    }
};

// generic swap specialization
// template <class T, size_t N>
// void swap(SmallPtr<T, N>& lhs, SmallPtr<T, N>& rhs)
//{
//    lhs.swap(rhs);
//}

using std::swap;

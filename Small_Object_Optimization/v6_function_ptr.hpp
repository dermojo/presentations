/**
 * @file    v6_function_ptr.cpp
 * @brief   version 6 of SmallPtr: avoid vtable overhead to save size by using function pointers
 */

#pragma once

#include <type_traits>
#include <utility>

enum class Action
{
    get_const,
    get_nonconst,
    move_to,
    uses_heap,
    destroy
};

template <class T>
struct ParamTypes
{
    struct GetConst
    {
        const void* stack; // in
        const T* ptr;      // out
    };
    struct GetNonConst
    {
        void* stack; // in
        T* ptr;      // out
    };
    struct MoveTo
    {
        void* stackFrom;
        void* stackTo;
    };
    struct UsesHeap
    {
        bool value;
    };
    struct Destroy
    {
        void* stack;
    };

    using Param = union {
        GetConst getConst;
        GetNonConst getNonConst;
        MoveTo moveTo;
        UsesHeap usesHeap;
        Destroy destroy;
    };
};


template <class Derived, class Base>
class StackStorage
{
public:
    using Params = ParamTypes<Base>;
    static void execute(Action action, typename Params::Param& param)
    {
        switch (action)
        {
        case Action::get_const:
            get(param.getConst);
            break;
        case Action::get_nonconst:
            get(param.getNonConst);
            break;
        case Action::move_to:
            moveTo(param.moveTo);
            break;
        case Action::uses_heap:
            usesHeap(param.usesHeap);
            break;
        case Action::destroy:
            destroy(param.destroy);
            break;
        default:
            break;
        }
    }

    static void moveTo(typename Params::MoveTo& param)
    {
        Derived* fromPtr = reinterpret_cast<Derived*>(param.stackFrom);
        ::new (param.stackTo) Derived(std::move(*fromPtr));
    }
    static void get(typename Params::GetNonConst& param)
    {
        param.ptr = reinterpret_cast<Derived*>(param.stack);
    }
    static void get(typename Params::GetConst& param)
    {
        param.ptr = reinterpret_cast<const Derived*>(param.stack);
    }
    static void usesHeap(typename Params::UsesHeap& param) { param.value = false; }
    static void destroy(typename Params::Destroy& param)
    {
        Derived* ptr = reinterpret_cast<Derived*>(param.stack);
        ptr->~Derived();
    }
};

template <class Derived, class Base>
class HeapStorage
{
public:
    using Params = ParamTypes<Base>;
    static void execute(Action action, typename Params::Param& param)
    {
        switch (action)
        {
        case Action::get_const:
            get(param.getConst);
            break;
        case Action::get_nonconst:
            get(param.getNonConst);
            break;
        case Action::move_to:
            moveTo(param.moveTo);
            break;
        case Action::uses_heap:
            usesHeap(param.usesHeap);
            break;
        case Action::destroy:
            destroy(param.destroy);
            break;
        default:
            break;
        }
    }

    static void moveTo(typename Params::MoveTo& param)
    {
        memcpy(param.stackTo, param.stackFrom, sizeof(Base*));
        set(param.stackFrom, nullptr);
    }
    static void get(typename Params::GetNonConst& param)
    {
        Derived* ptr;
        memcpy(&ptr, param.stack, sizeof(Base*));
        param.ptr = ptr;
    }
    static void get(typename Params::GetConst& param)
    {
        const Derived* ptr;
        memcpy(&ptr, param.stack, sizeof(Base*));
        param.ptr = ptr;
    }
    static void usesHeap(typename Params::UsesHeap& param) { param.value = true; }
    static void destroy(typename Params::Destroy& param)
    {
        Derived* ptr;
        memcpy(&ptr, param.stack, sizeof(Base*));
        if (ptr)
        {
            delete ptr;
            set(param.stack, nullptr);
        }
    }
    static void set(void* stack, Derived* ptr) { memcpy(stack, &ptr, sizeof(ptr)); }
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

    using Params = typename ParamTypes<T>::Param;
    using StorageFunc = void (*)(Action, Params&);

    StorageFunc m_ptr;
    typename std::aligned_storage<T_StackSize, alignment>::type m_stack;

    // tag dispatching
    struct stack_tag
    {
    };
    struct heap_tag
    {
    };

    T* getPtr() noexcept
    {
        if (m_ptr)
        {
            Params p;
            p.getNonConst.stack = &m_stack;
            m_ptr(Action::get_nonconst, p);
            return p.getNonConst.ptr;
        }
        return nullptr;
    }
    const T* getPtr() const noexcept
    {
        if (m_ptr)
        {
            Params p;
            p.getConst.stack = &m_stack;
            m_ptr(Action::get_const, p);
            return p.getConst.ptr;
        }
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
            Params p;
            p.destroy.stack = &m_stack;
            m_ptr(Action::destroy, p);
            m_ptr = nullptr;
        }
        if (ptr)
        {
            storePtr<T>(ptr);
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

    bool usesHeap() const noexcept
    {
        if (!m_ptr)
            return false;
        Params p;
        m_ptr(Action::uses_heap, p);
        return p.usesHeap.value;
    }
    bool usesStack() const noexcept { return !usesHeap(); }

    template <class Derived, class... Args>
    void emplace(Args&&... args)
    {
        static_assert(std::is_same<T, Derived>::value or std::is_base_of<T, Derived>::value,
                      "may only use sub-classes of T!");
        static_assert(std::is_constructible<Derived, Args...>::value, "cannot instantiate!");

        reset();
        constexpr bool smallFit = (sizeof(Derived) <= T_StackSize);
        emplaceImpl<Derived>(std::conditional_t < smallFit &&
                               std::is_move_constructible<Derived>::value,
                             stack_tag, heap_tag > {}, std::forward<Args>(args)...);
    }

private:
    template <class Derived, class... Args>
    void emplaceImpl(stack_tag, Args&&... args)
    {
        void* stack = &m_stack;
        ::new (stack) Derived(std::forward<Args>(args)...);
        m_ptr = StackStorage<Derived, T>::execute;
    }
    template <class Derived, class... Args>
    void emplaceImpl(heap_tag, Args&&... args)
    {
        Derived* ptr = new Derived(std::forward<Args>(args)...);
        storePtr<Derived>(ptr);
    }

    template <class Derived>
    void storePtr(Derived* ptr)
    {
        m_ptr = HeapStorage<Derived, T>::execute;
        HeapStorage<Derived, T>::set(&m_stack, ptr);
    }

    void assign(SmallPtr<T, T_StackSize>& rhs) /* noexcept */
    {
        if (rhs.m_ptr)
        {
            Params p;
            p.moveTo.stackFrom = &rhs.m_stack;
            p.moveTo.stackTo = &m_stack;
            rhs.m_ptr(Action::move_to, p);
            m_ptr = rhs.m_ptr;
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

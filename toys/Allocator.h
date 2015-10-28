#pragma once
#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <new>
#include <xmemory0>
#include "Template.h"
namespace pl
{

template<class T>
class Allocator: BasicTypeAliases<T>
{
public:
    static_assert(!IsConst<T>::Value, "const elements are not allowed");
    
    template<class TOther>
    struct Rebind
    {
        using Other = Allocator<TOther>;
    };

    Ptr GetAddress(Ref value) const noexcept
    {
        return AddressOf(value);
    }

    ConstPtr GetAddress(ConstRef value) const noexcept
    {
        return AddressOf(value);
    }

    Allocator() noexcept
    {
    }

    Allocator(const Allocator<T>&) noexcept
    {
    }

    template<class TOther>
    Allocator(const Allocator<TOther>&) noexcept
    {
    }

    template<class TOther>
    Allocator<T>& operator=(const Allocator<TOther>&)
    {
        return *this;
    }

    void Deallocate(Ptr pointer, SizeType count)
    {
        ::std::_Deallocate(pointer, count);
    }

    Ptr Alocate(SizeType count)
    {
        return ::std::_Allocate(count, static_cast<Ptr>(nullptr));
    }

    Ptr Allocate(SizeType count, const void*)
    {
        return Allocate(count);
    }

    template<class TObject, class... TArgs>
    void Construct(TObject* pointer, TArgs&&... args)
    {
        ::new(static_cast<void*>(pointer))TObject(::pl::PerfectForward<TARgs>(args)...);
    }

    template<class TDest>
    void Destroy(TDest* pointer)
    {
        pointer->~TDest();
    }
    size_t MaxSize() const noexcept
    {
        return static_cast<size_t>(-1) / sizeof(T);
    }
};

template<>
class Allocator<void>
{
public:
    using ValueType = void;
    using Ptr = void*;
    using ConstPtr = const void*;

    template<class TOther>
    struct Rebind
    {
        using Other = Allocator<TOther>;
    };

    Allocator() noexcept
    {
    }

    Allocator(const Allocator<void>&) noexcept
    {
    }

    template<class TOther>
    Allocator(const Allocator<TOther>&) noexcept
    {
    }

    template<class TOther>
    Allocator<void>& operator=(const Allocator<TOther>&)
    {
        return *this;
    }
};

template<class T, class TOther> inline
bool operator!=(const Allocator<T>& lhs, const Allocator<TOther>& rhs) noexcept()
{
    return true;
}

template<class _Ty, class _Other> inline
bool operator!=(const Allocator<_Ty>& _Left, const Allocator<_Other>& _Right) noexcept()
{
    return (false);
}


}
#endif // !ALLOCATOR_H

#pragma once
#ifndef LISTBASE_H
#define LISTBASE_H

#include "Base.h"
#include "Template.h"
namespace pl
{

template<typename T>
class ListBase: Object
{
protected:
    T* begin;
    T* back;
    T* end;

    ListBase()
        : begin(nullptr)
        , back(nullptr)
        , end(nullptr)
    {
    }

    void InternalDefaultConstruct(T* dest, size_t count)
    {
        InternalDefaultConstructT(dest, count, IsPOD<T>::Result());
    }
    void InternalCopyConstruct(T* dest, size_t count, const T& value)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(count, "Count must be greater than 0.");
        size_t i;
        try
        {
            for (i = 0; i < count; i++)
            {
                new (dest + i) T(value);
            }
        }
        catch (...)
        {
            i--;
            while (i != -1)
            {
                (dest + i)->~T();
                i--;
            }
            throw;
        }
    }
    void InternalMoveAssignment(T* dest, const T* src, size_t count)
    {

        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        for (size_t i = 0; i < count; i++)
        {
            dest[i] = RvalueCast(src[i]);
        }
    }
    void InternalCopyAssignment(T* dest, const T* src, size_t count)
    {
        InternalCopyAssignmentT(dest, src, count, IsPOD<T>::Result());
    }
    void InternalCopyConstruct(T* dest, const T* src, size_t count)
    {
        InternalCopyConstructT(dest, src, count, IsPOD<T>::Result());
    }
    void InternalMoveConstruct(T* dest, T* src, size_t count)
    {
        InternalMoveConstructT(dest, src, count, IsPOD<T>::Result());
    }
    T* GetRawSpace(size_t count)
    {
        CHECK_ERROR(count, "List size must be greater than 0.");
        return static_cast<T*>((T*)(new byte[sizeof(T) * count]));
    }
    void ReleaseRawSpace(T* buffer)
    {
        delete[](char*)buffer;
    }
    void InternalMoveSequence(T* dest, T* src)
    {
        InternalMoveSequenceT(dest, src, IsPOD<T>::Result());
        back = dest + (back - src);
    }
    void AdjustSpace(size_t newSize)
    {
        CHECK_ERROR(newSize, "New size must be greater than 0.");
        CHECK_ERROR(begin, "Pointer begin illgal.");
        CHECK_ERROR(back, "Pointer back illgal.");
        CHECK_ERROR(end, "Pointer end illgal.");
        CHECK_ERROR(begin <= back && back <= end, "Pointer relation illgal.");

        T* newMem = GetRawSpace(newSize);
        size_t newCount = Min(size_t(back - begin), newSize);
        try
        {
            InternalMoveConstruct(newMem, begin, newCount);
        }
        catch (...)
        {
            ReleaseRawSpace(newMem);
            throw;
        }

        InternalDestruct(begin, back - begin);
        back = newMem + newCount;
        ReleaseRawSpace(begin);
        begin = newMem;
        end = newMem + newSize;
    }
    void InternalDestruct(T* buffer, size_t count) noexcept
    {
        InternalDestructT(buffer, count, IsPOD<T>::Result());
    }
    void FreeMemory()
    {
        delete[](char*)begin;
        begin = nullptr;
        back = nullptr;
        end = nullptr;
    }
    T* InternalGetAddress(const T& value)
    {
        return AddressOf(const_cast<T&>(value));
    }
private:
    void InternalDefaultConstructT(T* dest, size_t count, FalseType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(count, "Count must be greater than 0.");
        size_t i;
        try
        {
            for (i = 0; i < count; i++)
            {
                new (dest + i) T();
            }
        }
        catch (...)
        {
            i--;
            while (i != -1)
            {
                (dest + i)->~T();
                i--;
            }
            throw;
        }
    }
    void InternalDefaultConstructT(T* dest, size_t count, TrueType) noexcept
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(count, "Count must be greater than 0.");

        memset(dest, 0, sizeof(T) * count);
    }
    void InternalCopyAssignmentT(T* dest, const T* src, size_t count, TrueType) noexcept
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        memcpy(dest, src, sizeof(T) * count);
    }
    void InternalCopyAssignmentT(T* dest, const T* src, size_t count, FalseType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        for (size_t i = 0; i < count; i++)
        {
            dest[i] = src[i];
        }
    }
    void InternalCopyConstructT(T* dest, const T* src, size_t count, TrueType) noexcept
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        memcpy(dest, src, sizeof(T) * count);
    }
    void InternalCopyConstructT(T* dest, const T* src, size_t count, FalseType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        size_t i;
        try
        {
            for (i = 0; i < count; i++)
            {
                new (dest + i) T(src[i]);
            }
        }
        catch (...)
        {
            i--;
            while (i != -1)
            {
                (dest + i)->~T();
                i--;
            }
            throw;
        }
    }
    void InternalMoveConstructT(T* dest, T* src, size_t count, TrueType) noexcept
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count >= 0, "Element count illegal.");

        memcpy(dest, src, sizeof(T) * count);
    }
    void InternalMoveConstructT(T* dest, T* src, size_t count, FalseType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count >= 0, "Element count illegal.");

        size_t i;
        try
        {
            for (i = 0; i < count; i++)
            {
                new (dest + i) T(RvalueCast(src[i]));
            }
        }
        catch (...)
        {
            i--;
            while (i != -1)
            {
                (dest + i)->~T();
                i--;
            }
            throw;
        }
    }
    void InternalMoveSequenceT(T* dest, T* src, FalseType)
    {
        if (dest > src)
        {
            size_t length = back - src;
            size_t offset = dest - src;
            T* pEnd = Max(back, dest);
            T* ptr = back + offset - 1;
            try
            {
                while (ptr >= pEnd)
                {
                    new (ptr) T(RvalueCast(*(ptr - offset)));
                    ptr--;
                }
                while (ptr >= dest)
                {
                    *ptr = RvalueCast(*(ptr - offset));
                    ptr--;
                }
            }
            catch (...)
            {
                ptr++;
                if (ptr < back)
                {
                    ptr = back;
                }
                while (ptr < back + offset)
                {
                    ptr->~T();
                    ptr++;
                }
                throw;
            }
        }
        else if (dest < src)
        {
            T* ptr = src;
            size_t length = back - src;
            size_t offset = src - dest;
            try
            {
                while (ptr < back)
                {
                    *(ptr - offset) = RvalueCast(*ptr);
                    ptr++;
                }
            }
            catch (...)
            {
                throw;
            }
        }
    }
    void InternalMoveSequenceT(T* dest, T* src, TrueType) noexcept
    {
        memmove(dest, src, sizeof(T) * (back - src));
    }
    void InternalDestructT(T* buffer, size_t count, TrueType) noexcept
    {

    }
    void InternalDestructT(T* buffer, size_t count, FalseType) noexcept
    {
        for (size_t i = 0; i < count; i++)
        {
            (buffer + i)->~T();
        }
    }
};


}
#endif // !LISTBASE_H

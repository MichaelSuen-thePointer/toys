#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
#include <memory.h>
#include <new>
namespace pl
{
namespace container
{

template<typename T>
class List: public Object
{
protected:
    T* begin;
    T* back;
    T* end;

    static void InternalDefaultConstruct(T* dest, size_t count)
    {
        InternalDefaultConstructT(dest, count, IsPOD<T>::Result());
    }

    static void InternalDefaultConstructT(T* dest, size_t count, FalseType)
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
    static void InternalDefaultConstructT(T* dest, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(count, "Count must be greater than 0.");

        memset(dest, 0, count);
    }

    static void InternalCopyConstruct(T* dest, size_t count, const T& value)
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

    static void InternalCopyConstruct(T* dest, const T* src, size_t count)
    {
        InternalCopyConstructT(dest, src, count, IsPOD<T>::Result());
    }

    static void InternalCopyConstructT(T* dest, const T* src, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        memcpy(dest, src, count);
    }
    static void InternalCopyConstructT(T* dest, const T* src, size_t count, FalseType)
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

    static void InternalCopy(T* dest, const T* src, size_t count)
    {
        InternalCopyT(dest, src, count, IsPOD<T>::Result());
    }

    static void InternalCopyT(T* dest, const T* src, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");
        memcpy(dest, src, count);
    }
    static void InternalCopyT(T* dest, const T* src, size_t count, FalseType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        for (size_t i = 0; i < count; i++)
        {
            dest[i] = src[i];
        }
    }
    static T* GetRawSpace(size_t count)
    {
        CHECK_ERROR(count, "List size must be greater than 0.");
        return static_cast<T*>((T*)(new byte[sizeof(T) * count]));
    }
    static void InternalMove(T* dest, const T* src, size_t count)
    {
        InternalMoveT(dest, src, count, IsPOD<T>::Result());
    }

    static void InternalMoveT(T* dest, const T* src, size_t count, FalseType)
    {
        if (src < dest)
        {
            size_t i = count - 1;
            while (i != -1)
            {
                dest[i] = RvalueCast(src[i]);
                i--;
            }
        }
        else if (src > dest)
        {
            size_t i;
            for (i = 0; i < count; i++)
            {
                dest[i] = RvalueCast(src[i]);
            }
        }
    }
    static void InternalMoveT(T* dest, const T* src, size_t count, TrueType)
    {
        memmove(dest, src, sizeof(T) * count);
    }
    void AdjustSpace(size_t newSize)
    {
        CHECK_ERROR(newSize, "New size must be greater than 0.");
        T* newMem = GetRawSpace(newSize);
        CHECK_ERROR(back - begin, "Member back is smaller than begin.");
        size_t newCount = Min(size_t(back - begin), newSize);
        try
        {
            InternalCopyConstruct(newMem, begin, newCount);
        }
        catch (...)
        {
            delete[](char*)newMem;
            throw;
        }

        InternalDestruct(begin, back - begin);
        back = newMem + newCount;
        delete[](char*)begin;
        begin = newMem;
        end = newMem + newSize;
    }

    static void InternalDestruct(T* buffer, size_t count)
    {
        InternalDestructT(buffer, count, IsPOD<T>::Result());
    }

    static void InternalDestructT(T* buffer, size_t count, TrueType)
    {
    }

    static void InternalDestructT(T* buffer, size_t count, FalseType)
    {
        for (size_t i = 0; i < count; i++)
        {
            (buffer + i)->~T();
        }
    }

    void FreeMemory()
    {
        delete[](char*)begin;
        begin = nullptr;
        back = nullptr;
        end = nullptr;
    }
public:
    List()
        : List(20)
    {
    }

    List(size_t count)
        : begin(GetRawSpace(count))
        , back(begin + count)
        , end(begin + count)
    {
        try
        {
            InternalDefaultConstruct(begin, count);
        }
        catch (...)
        {
            FreeMemory();
            throw;
        }
    }

    List(size_t count, const T& value)
        : begin(GetRawSpace(count))
        , back(begin + count)
        , end(begin + count)
    {
        try
        {
            InternalCopyConstruct(begin, count, value);
        }
        catch (...)
        {
            FreeMemory();
            throw;
        }
    }

    List(T* first, T* last)
        : begin(GetRawSpace(last - first))
        , back(begin + (last - first))
        , end(begin + last - first)
    {
        try
        {
            InternalCopyConstruct(begin, first, last - first);
        }
        catch (...)
        {
            FreeMemory();
            throw;
        }
    }
    List(const List& list)
        : begin(GetRawSpace(list.end - list.begin))
        , back(begin + (end - begin))
        , end(begin + list.Capacity())
    {
        try
        {
            InternalCopyConstruct(begin, list.begin, list.back - list.begin);
        }
        catch (...)
        {
            FreeMemory();
            throw;
        }
    }
    List(List&& list)
        : begin(list.begin)
        , back(list.back)
        , end(list.end)
    {
        list.begin = nullptr;
        list.back = nullptr;
        list.end = nullptr;
    }

    size_t Count() const
    {
        return back - begin;
    }

    size_t Capacity() const
    {
        return end - begin;
    }

    void AddBack(const T& value)
    {
        if (back == end)
        {

            AdjustSpace(Capacity() + Capacity() / 5 + 1);
        }
        new (back) T(value);
        back++;
    }

    void AddBack(T&& value)
    {
        if (back == end)
        {
            AdjustSpace(Capacity() + Capacity() / 5 + 1);
        }
        new (back) T(RvalueCast(value));
        back++;
    }

    void RemoveBack()
    {
        CHECK_ERROR(back != begin, "List is empty.");
        back--;
        back->~T();
    }

    void Insert(size_t place, const T* first, const T* last)
    {
        CHECK_ERROR(begin + place <= back, "Parameter place is invalid.");
        CHECK_ERROR(first && last && first <= last, "Range invalid.");

        size_t insertCount = last - first;
        if (insertCount + back > end)
        {
            AdjustSpace(insertCount + Count());
        }
        InternalMove(begin + place + insertCount, begin + place, Count() - place);
        InternalDestruct(begin + place, Count() - place);
        InternalCopyConstruct(begin + place, first, insertCount);
    }

    void Insert(size_t place, size_t count, const T& value)
    {
        if (end - back < count)
        {
            AdjustSpace(end - begin + count);
        }
        InternalMove(begin + place + count, begin + place, count);
        InternalDestruct(begin + place, count);
        InternalCopyConstruct(begin + place, count, value);
    }

    void Insert(size_t place, const T& value)
    {
        if (end - back < 1)
        {
            AdjustSpace(end - begin + 1);
        }
        InternalMove(begin + place + 1, begin + place, 1);
        InternalDestruct(begin + place, 1);
        new (begin + place) T(value);
    }

    void Insert(size_t place, T&& value)
    {
        if (end - back < 1)
        {
            AdjustSpace(end - begin + 1);
        }
        InternalMove(begin + place + 1, begin + place, 1);
        InternalDestruct(begin + place, 1);
        new (begin + place) T(RvalueCast(value));
    }

    T& operator[](size_t index)
    {
        return begin[index];
    }

    const T& operator[](size_t index) const
    {
        return begin[index];
    }

    ~List()
    {
        if (begin)
        {
            InternalDestruct(begin, back - begin);
            FreeMemory();
        }
    }
};

}
}
#endif // !LIST_H

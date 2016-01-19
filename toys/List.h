#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
#include <string.h>
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
    T* value_end;
    T* real_end;

    static void InternalDefaultConstruct(T* dest, size_t count, FalseType)
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
                dest[i].~T();
                i--;
            }
            throw;
        }
    }
    static void InternalDefaultConstruct(T* dest, size_t count, TrueType)
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
                dest[i].~T();
                i--;
            }
            throw;
        }
    }
    static void InternalCopyConstruct(T* dest, T* src, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");

        memcpy(dest, src, count);
    }
    static void InternalCopyConstruct(T* dest, T* src, size_t count, FalseType)
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
                dest[i].~T();
                i--;
            }
            throw;
        }
    }

    static void InternalCopy(T* dest, T* src, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(src, "Source pointer illegal.");
        CHECK_ERROR(count, "Element count illegal.");
        memcpy(dest, src, count);
    }
    static void InternalCopy(T* dest, T* src, size_t count, FalseType)
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
    static void InternalMove(T* dest, T* src, size_t count, TrueType)
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

    void AdjustSpace(size_t newSize)
    {
        CHECK_ERROR(newSize, "New size must be greater than 0.");
        T* newMem = GetRawSpace(size_t newSize);
        try
        {
            InternalCopy(newMem, begin, Min(real_end - begin, newSize), IsPOD<T>::Result());
        }
        catch (...)
        {
            delete[](char*)newMem;
            throw;
        }

        value_end = newMem + value_end - begin;
        real_tail = newMem + Min(real_tail - begin, newSize);
        delete[](char*)begin;
        begin = newMem;
        real_end = newMem + newSize;
    }

    void CleanUpMess()
    {
        delete[](char*)begin;
        begin = nullptr;
        value_end = nullptr;
        real_tail = nullptr;
        real_end = nullptr;
    }
public:
    List()
        : List(20)
    {
    }

    List(size_t count)
        : begin(GetRawSpace(count))
        , value_end(begin + count)
        , real_end(begin)
    {
        try
        {
            InternalDefaultConstruct(begin, count, IsPOD<T>::Result());
        }
        catch (...)
        {
            CleanUpMess();
            throw;
        }
    }

    List(size_t count, const T& value)
        : begin(GetRawSpace(count))
        , value_end(begin + count)
        , real_end(begin)
    {
        try
        {
            InternalCopyConstruct(begin, count, value);
        }
        catch (...)
        {
            CleanUpMess();
            throw;
        }
    }

    List(T* begin, T* end)
        : begin(GetRawSpace(end - begin))
        , value_end(begin + (end - begin))
        , real_end(begin)
    {
        try
        {
            InternalCopyConstruct(begin, begin, end - begin, IsPOD<T>::Result());
        }
        catch (...)
        {
            CleanUpMess();
            throw;
        }
    }
    List(const List& list)
        : begin(GetRawSpace(list.real_end - list.begin))
        , value_end(begin + (real_end - begin))
        , real_end(begin)
    {
        try
        {
            InternalCopyConstruct(begin, list.begin, list.value_end - list.begin, IsPOD<T>::Result());
        }
        catch (...)
        {
            CleanUpMess();
            throw;
        }
    }
    List(List&& list)
        : begin(list.begin)
        , value_end(list.value_end)
        , real_end(list.real_end)
    {
        list.begin = nullptr;
        list.value_end = nullptr;
        list.real_end = nullptr;
    }

    size_t Count() const
    {
        return value_end - begin;
    }

    size_t Capacity() const
    {
        return real_end - begin;
    }

    void AddBack(const T& value)
    {

    }

    void AddBack(T&& value)
    {

    }
    void RemoveBack()
    {
        CHECK_ERROR(value_end != begin, "List is empty.");
        value_end--;
    }

    T& operator[](size_t index)
    {
        return begin[index];
    }
    const T& operator[](size_t index)
    {
        return begin[index];
    }

    ~List()
    {
        if (begin)
        {
            size_t i = value_end - begin - 1;
            while (i != -1)
            {
                begin[i].~T();
                i--;
            }
            CleanUpMess();
        }
    }
};

}
}
#endif // !LIST_H

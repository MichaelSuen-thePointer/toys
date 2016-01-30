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

    void InternalDefaultConstruct(T* dest, size_t count)
    {
        InternalDefaultConstructT(dest, count, IsPOD<T>::Result());
    }

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

    void InternalDefaultConstructT(T* dest, size_t count, TrueType)
    {
        CHECK_ERROR(dest, "Destination pointer illegal.");
        CHECK_ERROR(count, "Count must be greater than 0.");

        memset(dest, 0, sizeof(T) * count);
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

    void InternalCopyAssignmentT(T* dest, const T* src, size_t count, TrueType)
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

    void InternalCopyConstruct(T* dest, const T* src, size_t count)
    {
        InternalCopyConstructT(dest, src, count, IsPOD<T>::Result());
    }

    void InternalCopyConstructT(T* dest, const T* src, size_t count, TrueType)
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

    void InternalMoveConstruct(T* dest, T* src, size_t count)
    {
        InternalMoveConstructT(dest, src, count, IsPOD<T>::Result());
    }

    void InternalMoveConstructT(T* dest, T* src, size_t count, TrueType)
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

    void InternalMoveSequenceT(T* dest, T* src, TrueType)
    {
        memmove(dest, src, sizeof(T) * (back - src));
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

    List<T>& operator=(const List<T>& list)
    {
        if (&list != this)
        {
            Assign(list);
        }
        return *this;
    }

    List<T>& operator=(List<T>&& list)
    {
        if (&list != this)
        {
            Assign(RvalueCast(list));
        }
        return *this;
    }

    void Assign(const List<T>& list)
    {
        if (list.Count() > this->Capacity())
        {
            InternalDestruct(begin, back - begin);
            back = begin;
            AdjustSpace(list.Capacity());
        }
        InternalCopyAssignment(begin, list.begin, list.Count());
        back = begin + list.Count();
    }

    void Assign(List<T>&& list)
    {
        InternalDestruct(begin, Count());
        FreeMemory();
        begin = list.begin;
        back = list.back;
        end = list.end;
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
            AdjustSpace(begin, back, Capacity() + Capacity() / 5 + 1);
        }
        InternalCopyConstruct(back, &value, 1);
        back++;
    }

    void AddBack(T&& value)
    {
        if (back == end)
        {
            AdjustSpace(Capacity() + Capacity() / 5 + 1);
        }
        InternalMoveConstruct(back, &value, 1);
        back++;
    }

    void RemoveBack()
    {
        CHECK_ERROR(back != begin, "List is empty.");
        back--;
        InternalDestruct(back, 1);
    }

    void Insert(size_t place, const T* first, const T* last)
    {
        CHECK_ERROR(begin + place <= back, "Parameter place is invalid.");
        CHECK_ERROR(first && last && first <= last, "Range invalid.");

        size_t count = last - first;
        size_t oldCount = back - begin;
        if (count + back > end)
        {
            AdjustSpace(count + Count());
        }
        InternalMoveSequence(begin + place + count, begin + place);
        InternalCopyAssignment(begin + place, first, count);
    }

    void Insert(size_t place, size_t count, const T& value)
    {
        if (end - back < count)
        {
            AdjustSpace(end - begin + count);
        }
        InternalMoveSequence(begin + place + count, begin + place);
        InternalCopyAssignment(begin + place, &value, count);
    }

    void Insert(size_t place, const T& value)
    {
        CHECK_ERROR(place >= 0 && place < back - begin, "Place illgal.");
        size_t originCount = back - begin;
        if (end - back < 1)
        {
            AdjustSpace(end - begin + 1);
        }
        InternalMoveSequence(begin + place + 1, begin + place);
        InternalCopyAssignment(begin + place, &value, 1);
    }

    void Insert(size_t place, T&& value)
    {
        CHECK_ERROR(place >= 0 && place < back - begin, "Place illgal.");
        size_t originCount = back - begin;
        if (end - back < 1)
        {
            AdjustSpace(end - begin + 1);
        }
        InternalMoveSequence(begin + place + 1, begin + place);
        InternalMoveAssignment(begin + place, &value, 1);
    }

    T& operator[](size_t index)
    {
        CHECK_ERROR(index >= 0 && index < back - begin, "Index illgal.");
        return begin[index];
    }

    const T& operator[](size_t index) const
    {
        CHECK_ERROR(index >= 0 && index < back - begin, "Index illgal.");
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

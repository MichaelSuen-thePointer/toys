#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
#include "ListBase.h"
#include <memory.h>
#include <new>
namespace pl
{
namespace container
{

template<typename T>
class List: public ListBase<T>
{
public:
    List()
        : List(20)
    {
    }

    List(size_t count)
    { 
        begin = GetRawSpace(count);
        back = begin + count;
        end = begin + count
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
    {
        begin = GetRawSpace(count);
        back = begin + count;
        end = begin + count;
    
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
    {
        begin = GetRawSpace(last - first);
        back = begin + (last - first);
        end = begin + last - first;
    
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
    {
        begin = GetRawSpace(list.end - list.begin);
        back = begin + (end - begin);
        end = begin + list.Capacity();
    
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
    {
        begin = list.begin;
        back = list.back;
        end = list.end;
    
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
        if (this != &list)
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
    }

    void Assign(List<T>&& list)
    {
        if (this != list)
        {
            InternalDestruct(begin, back - begin);
            FreeMemory();
            begin = list.begin;
            back = list.back;
            end = list.end;
            list.begin = nullptr;
            list.back = nullptr;
            list.end = nullptr;
        }
    }

    void Reserve(size_t newSize)
    {
        AdjustSpace(newSize);
    }

    void ShrinkToFit() const
    {
        AdjustSpace(back - begin);
    }

    T& First()
    {
        CHECK_ERROR(begin != nullptr && back > begin, "List is Empty");
        return *begin;
    }

    const T& First() const
    {
        CHECK_ERROR(begin != nullptr && back > begin, "List is Empty");
        return *begin;
    }

    T& Last()
    {
        CHECK_ERROR(begin != nullptr && back > begin, "List is Empty");
        return back[-1];
    }

    const T& Last() const
    {
        CHECK_ERROR(begin != nullptr && back > begin, "List is Empty");
        return back[-1];
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
        InternalCopyConstruct(back, InternalGetAddress(value), 1);
        back++;
    }

    void AddBack(T&& value)
    {
        if (back == end)
        {
            AdjustSpace(Capacity() + Capacity() / 5 + 1);
        }
        InternalMoveConstruct(back, InternalGetAddress(value), 1);
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
        InternalCopyAssignment(begin + place, InternalGetAddress(value), count);
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
        InternalCopyAssignment(begin + place, InternalGetAddress(value), 1);
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
        InternalMoveAssignment(begin + place, InternalGetAddress(value), 1);
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

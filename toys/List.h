#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
namespace pl
{
namespace container
{
template<class T, typename IsPOD>
class ListBase: public Object
{};

template<class T>
class ListBase<T, POD>: public Object
{
protected:
    T* _array;
    size_t _size;

    void InternalConstruct(size_t count)
    { //default set sero
        memset(_array, 0, count * sizeof(T));
    }
    void InternalConstruct(size_t count, T& value)
    {
        for (int i = 0; i < count; i++)
        {
            _array[i] = value;
        }
    }
    void InternalCopy(T* source, size_t count)
    { //is pod type
        memcpy(_array, source, count * sizeof(T));
    }
    ListBase(size_t length, T& value)
        : _array(new T[length])
        , _size(length)
    {
        InternalConstruct(length, value)
    }
    ListBase(size_t length)
        : _array(new T[length])
        , _size(length)
    {
        InternalConstruct(length);
    }
    ListBase(const ListBase<T, POD>& listbase)
        : _array(new T[listbase._size])
        , _size(listbase._size)
    {
        InternalCopy(listbase._array, listbase._size);
    }
    ListBase(ListBase<T, POD>&& listbase)
        : _array(listbase._array)
        , _size(listbase._size)
    {
        listbase._raw_memory = nullptr;
        listbase._array = nullptr;
        listbase._size = 0;
    }
    ~ListBase()
    {
        delete[] _array;
    }
    ListBase<T, POD>& operator=(const ListBase<T, POD>& listbase)
    {
        InternalCopy(listbase._array, listbase._size);
        _size = listbase._size;
        return *this;
    }
    ListBase<T, POD>& operator=(ListBase<T, POD>&& listbase)
    {
        _array = listbase._array;
        _size = listbase._size;
        listbase._array = nullptr;
        listbase._size = 0;
        return *this;
    }
};

template<class T>
class ListBase<T, NotPOD>: public Object
{
protected:
    byte* _raw_memory;
    T* _array;
    size_t _size;

    void InternalConstruct(size_t count, T& value)
    {
        for (size_t i = 0; i < count; i++)
        {
            new (_array + i)T(value);
        }
    }
    void InternalConstruct(T* source, size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            new (_array + i)T(source[i]);
        }
    }
    void InternalCopy(T* source, size_t count)
    { //is not pod type
        for (size_t i = 0; i < count; i++)
        {
            _array[i] = source[i];
        }
    }
    void InternalDestruct(size_t count)
    {
        for (size_t i = 0; i < count; i++)
        {
            _array[i].~T();
        }
    }
    ListBase(size_t length)
        : ListBase(length, T())
    {
    }
    ListBase(size_t length, T& value)
        : _raw_memory(new byte[length * sizeof(T)])
        , _array(_raw_memory)
        , _size(length)
    {
        InternalConstruct(length, value)
    }
    ListBase(const ListBase<T, POD>& listbase)
        : _raw_memory(new byte[listbase._size * sizeof(T)])
        , _array(_raw_memory)
        , _size(listbase._size)
    {
        InternalConstruct(listbase._array, listbase._size);
    }
    ListBase(ListBase<T, POD>&& listbase)
        : _raw_memory(listbase._raw_memory)
        , _array(listbase._array)
        , _size(listbase._size)
    {
        listbase._raw_memory = nullptr;
        listbase._array = nullptr;
        listbase._size = 0;
    }
    ~ListBase()
    {
        if (_raw_memory)
        {
            InternalDestruct(_size);
            delete[] _raw_memory;
        }
    }
    ListBase<T, POD>& operator=(const ListBase<T, POD>& listbase)
    {
        InternalCopy(listbase._array, listbase._size);
        _size = listbase._size;
        return *this;
    }
    ListBase<T, POD>& operator=(ListBase<T, POD>&& listbase)
    {
        _array = listbase._array;
        _size = listbase._size;
        listbase._array = nullptr;
        listbase._size = 0;
        return *this;
    }
};

template<class T>
class FixSizeList: public ListBase<T, typename IsPOD<T>::Result>
{
public:
    FixSizeList(size_t length)
        : ListBase(length)
    {
    }
    FixSizeList(size_t length, T value)
        : ListBase(length, value)
    {
    }
    FixSizeList(const FixSizeList<T>& fslist)
        : ListBase(fslist)
    {
    }
};


}
}
#endif // !LIST_H

#pragma once
#ifndef LIST_H
#define LIST_H

#include "Base.h"
#include "Template.h"
namespace pl
{
namespace container
{

/*ListBase for non-POD type*/

template<class T, typename IsPOD>
class ListBase: public Object
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
    void InternalFree()
    {
        InternalDestruct(_size);
        delete[] _raw_memory;
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
            InternalFree();
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
        InternalFree();
        _array = listbase._array;
        _size = listbase._size;
        listbase._array = nullptr;
        listbase._size = 0;
        return *this;
    }
    T& operator[](size_t index)
    {
        assert(index > 0 && index < _size);
        return _array[index];
    }
    const T& operator[](size_t index) const
    {
        assert(index > 0 && index < _size);
        return _array[index];
    }
};

/*ListBase for POD type*/

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
        delete[] _array;
        _array = listbase._array;
        _size = listbase._size;
        listbase._array = nullptr;
        listbase._size = 0;
        return *this;
    }
    T& operator[](size_t index)
    {
        assert(index > 0 && index < _size);
        return _array[index];
    }
    const T& operator[](size_t index) const
    {
        assert(index > 0 && index < _size);
        return _array[index];
    }
};

/*specialization for ListBase<bool>*/

template<>
class ListBase<bool, POD>: public Object
{
protected:
    static const int _bits_per_entry = sizeof(int) * 8;
    int* _array;
    size_t _size;
    size_t RealSize(size_t count)
    {
        return count / _bits_per_entry + !!(count / _bits_per_entry);
    }
    void InternalConstruct(size_t count)
    { //default set sero
        memset(_array, 0, RealSize(count) * sizeof(int));
    }
    void InternalConstruct(size_t count, bool& value)
    {
        if (value == false)
        {
            InternalConstruct(count);
        }
        else
        {
            memset(_array, -1, RealSize(count) * sizeof(int));
        }
    }
    void InternalCopy(int* source, size_t size)
    { //is pod type
        memcpy(_array, source, size * sizeof(int));
    }
public:
    class ListBoolProxy
    {
    private:
        int* _array;
        size_t _index;
    public:
        ListBoolProxy(int* array, size_t index)
            : _array(array)
            , _index(index)
        {
        }
        bool GetValue() const
        {
            return GetBit(_array[_index / _bits_per_entry], _bits_per_entry - _index % _bits_per_entry - 1);
        }
        void SetValue(bool value)
        {
            if (value == true)
            {
                _array[_index / _bits_per_entry] |= 1U << _bits_per_entry - _index % _bits_per_entry - 1;
            }
            else
            {
                _array[_index / _bits_per_entry] &= ~(1U << _bits_per_entry - _index % _bits_per_entry - 1);
            }
                
        }
        operator bool() const
        {
            return GetValue();
        }
        ListBoolProxy& operator=(bool value)
        {
            SetValue(value);
        }

    };
    ListBase(size_t length, bool value)
        : _array(new int[RealSize(length)])
        , _size(RealSize(length))
    {
        InternalConstruct(length, value);
    }
    ListBase(size_t length)
        : _array(new int[RealSize(length)])
        , _size(RealSize(length))
    {
        InternalConstruct(length);
    }
    ListBase(const ListBase<bool, POD>& listbase)
        : _array(new int[listbase._size])
        , _size(listbase._size)
    {
        InternalCopy(listbase._array, listbase._size);
    }
    ListBase(ListBase<bool, POD>&& listbase)
        : _array(listbase._array)
        , _size(listbase._size)
    {
        listbase._array = nullptr;
        listbase._size = 0;
    }
    ~ListBase()
    {
        delete[] _array;
    }
    ListBase<bool, POD>& operator=(const ListBase<bool, POD>& listbase)
    {
        InternalCopy(listbase._array, listbase._size);
        _size = listbase._size;
        return *this;
    }
    ListBase<bool, POD>& operator=(ListBase<bool, POD>&& listbase)
    {
        delete[] _array;
        _array = listbase._array;
        _size = listbase._size;
        listbase._array = nullptr;
        listbase._size = 0;
        return *this;
    }
    ListBoolProxy& operator[](size_t index)
    {
        assert(index > 0 && index < _size);
        return ListBoolProxy(_array, index);
    }
    const ListBoolProxy& operator[](size_t index) const
    {
        assert(index > 0 && index < _size);
        return ListBoolProxy(_array, index);
    }
};

template<class T>
class FixSizeList: public ListBase<T, typename IsPOD<T>::Result>
{
private:
    using BaseType = ListBase<T, typename IsPOD<T>::Result>;
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
    FixSizeList(FixSizeList<T>&& fslist)
        : ListBase(PerfectForward<T>(fslist))
    {
    }
};


}
}
#endif // !LIST_H

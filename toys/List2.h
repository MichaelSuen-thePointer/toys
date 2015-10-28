#pragma once
#ifndef LIST2_H
#define LIST2_H

#include "Base.h"
#include "Template.h"
#include <exception>
namespace pl
{
template<typename T>
class List: BasicTypeAliases<T>
{
protected:
    Ptr _array;
    SizeType _length;
    SizeType _head;
public:
    List()
        : _array(nullptr)
        , _length(0)
        , _head(0)
    {
    }
    List(size_t length)
        : _array(new T[length])
        , _length(length)
        , _head(0)
    {
    }
    List(size_t length, const T& value)
        : _array(new T[length])
        , _length(length)
        , _head(0)
    {
        for (int i = 0; i < length; i++)
        {
            _array[i] = value;
        }
    }
    List(const List<T>& list)
        : _array(new T[list._length])
        , _length(list._length)
        , _head(list._head)
    {
        for (SizeType i = 0; i < list._length; i++)
        {
            _array[i] = list._array[i];
        }
    }
    List(List<T>&& list)
        : _array(list._array)
        , _length(list._length)
        , _head(list._head)
    {
        list._array = nullptr;
        list._length = 0;
        list._head = 0;
    }
    T& operator[](SizeType index)
    {
        return _array[index];
    }
    const T& operator[](SizeType index) const
    {
        return _array[index];
    }
    void Push(const T& value)
    {
        if (_head == _length)
        {
            Expand(_length * 2);
        }
        _array[_head++] = value;
    }
    void Pop()
    {
        if (_head == 0)
        {
            throw std::exception("pop function called in empty list");
        }
        _head--;
    }
    const T& First() const
    {
        return const_cast<List<T>*>(this).First();
    }
    T& First()
    {
        if (_head == 0)
        {
            throw std::exception("first function called in empty list");
        }
        return _array[0];
    }
    const T& Last() const
    {
        return const_cast<List<T>*>(this).Last();
    }
    T& Last()
    {
        if (_head == 0)
        {
            throw std::exception("last function called in empty list");
        }
        return _array[_head - 1];
    }
    SizeType Length() const
    {
        return _length;
    }
    ~List()
    {
        if (_array)
        {
            delete[] _array;
        }
    }
private:
    void Expand(SizeType newLength)
    {
        if (newLength == 0 && _length == 0)
        {
            newLength = 2;
        }
        Ptr newArray;
        newArray = new T[newLength];
        for (SizeType i = 0; i < _length; i++)
        {
            newArray[i] = _array[i];
        }
        if (_array)
        {
            delete[] _array;
        }
        _array = newArray;
        _length = newLength;
    }

};

}
#endif // !LIST2_H

#pragma once
#ifndef STRING_H
#define STRING_H

#include "Base.h"
#include <memory.h>
namespace pl
{

template<typename TChar>
class StringBase: public Object
{
public: //public fields
    static const StringBase<TChar> Empty;
    static const TChar Zero = TChar();

private: //private fields
    mutable TChar* _buffer;
    mutable volatile size_t* _refCount;
    mutable size_t _start;
    mutable size_t _length;
    mutable size_t _realLength;

public: //public member func
    static size_t CStringLength(const TChar* string)
    {
        CHECK_ERROR(string, "parameter `string cannot be nullptr");
        size_t size = 0;
        while (*string++)
        {
            size++;
        }
        return size;
    }
    static int Compare(const StringBase& lhs, const StringBase& rhs)
    {
        TChar* lBuf = lhs._buffer + lhs._start;
        TChar* rBuf = rhs._buffer + rhs._start;
        int diff;
        while (*lBuf && *rBuf)
        {
            diff = *lBuf - *rBuf;
            if (diff != 0)
            {
                return diff;
            }
            lBuf++;
            rBuf++;
        }
        return lhs._length - rhs._length;
    }
    static bool Equal(const StringBase& lhs, const StringBase& rhs)
    {
        if (lhs._length == rhs._length)
        {
            for (size_t i = 0; i < lhs._length; i++)
            {
                if (lhs._buffer[i] != rhs._buffer[i])
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }
private: //private member func
    void RefInc()
    {
        LOCKED_INC(_refCount);
    }
    void RefDec()
    {
        if (LOCKED_DEC(_refCount) == 0)
        {
            delete _refCount;
            delete[] _buffer;
        }
    }
public: /*ctor*/
    StringBase()
        : _buffer(new TChar[1])
        , _refCount(0)
        , _start(0)
        , _length(0)
        , _realLength(1)
    {
        _buffer[0] = Zero;
    }
    StringBase(const TChar* string, size_t start, size_t length)
        : _buffer(new TChar[length + 1])
        , _refCount(new size_t(1))
        , _start(start)
        , _length(length)
        , _realLength(length)
    {
        memcpy(_buffer, string + start, length * sizeof(TChar));
        _buffer[_length] = Zero;
    }
    StringBase(const TChar* string, bool copy = true)
        : _buffer(nullptr)
        , _refCount(new size_t(1))
        , _start(0)
        , _length(CStringLength(string))
        , _realLength(_length)
    {
        if (copy)
        {
            _buffer = new TChar[_length + 1];
            _buffer[_length] = Zero;
            memcpy(_buffer, string, _length * sizeof(TChar));
        }
        else
        {
            _buffer = const_cast<TChar*>(string);
        }
    }
    StringBase(const TChar* string, size_t length)
        : StringBase(string, 0, length)
    {
    }
    StringBase(const TChar* start, const TChar* end)
        : StringBase(start, 0, end - start)
    {
    }
    StringBase(TChar ch)
        : StringBase(1, ch)
    {
    }
    StringBase(size_t count, TChar ch)
        : _buffer(new TChar[count + 1])
        , _refCount(new size_t(1))
        , _start(0)
        , _length(count)
        , _realLength(count)
    {
        memset(_buffer, ch, _length * sizeof(TChar));
        _buffer[_length] = '\0';
    }
    StringBase(const StringBase& string, size_t start, size_t length)
        : _buffer(string._buffer)
        , _refCount(string._refCount)
        , _start(start)
        , _length(length)
        , _realLength(string._realLength)
    {
        CHECK_ERROR(start + length <= string._length, "Index out of range");
        RefInc();
    }
    StringBase(const StringBase& string)
        : StringBase(string, string._start, string._length)
    {
    }
    StringBase(StringBase&& string)
        : _buffer(string._buffer)
        , _refCount(string._refCount)
        , _start(string._start)
        , _length(string._length)
        , _realLength(string._realLength)
    {
        string._buffer = nullptr;
        string._refCount = nullptr;
    }
    StringBase(const StringBase& string, size_t start)
        : StringBase(string, start, string._length - start)
    {
    }
    ~StringBase()
    {
        if (_refCount)
        {
            RefDec();
        }
    }

    StringBase& operator=(const StringBase& string)
    {
        if (this != &string)
        {
            RefDec();
            _buffer = string._buffer;
            _refCount = string._refCount;
            _start = string._start;
            _length = string._length;
            _realLength = string._realLength;
            RefInc();
        }
        return *this;
    }
    StringBase& operator=(StringBase&& string)
    {
        if (this != &string)
        {
            RefDec();
            _buffer = string._buffer;
            _refCount = string._refCount;
            _start = string._start;
            _length = string._length;
            _realLength = string._realLength;

            string._buffer = nullptr;
            string._refCount = nullptr;
            string._start = 0;
            string._length = 0;
            string._realLength = 0;
        }
        return *this;
    }
private: //private ctor
    StringBase(const StringBase& concat1, const StringBase& concat2, int beforeIndex, int length)
        : _buffer(new TChar[beforeIndex + length + 1])
        , _refCount(new size_t(1))
        , _start(0)
        , _length(beforeIndex + length)
        , _realLength(beforeIndex + length)
    {
        memcpy(_buffer, concat1._buffer + concat1._start, beforeIndex * sizeof(TChar));
        memcpy(_buffer + beforeIndex, concat2._buffer + concat2._start, length * sizeof(TChar));
        _buffer[_length] = '\0';
    }
public: //public member func
    bool operator==(const StringBase& String) { return Equal(*this, String); }
    bool operator!=(const StringBase& String) { return !Equal(*this, String); }
    bool operator> (const StringBase& String) { return Compare(*this, String) > 0; }
    bool operator>=(const StringBase& String) { return Compare(*this, String) >= 0; }
    bool operator< (const StringBase& String) { return Compare(*this, String) < 0; }
    bool operator<=(const StringBase& String) { return Compare(*this, String) <= 0; }

    StringBase operator+(const StringBase& string)
    {
        return StringBase(*this, string, _length, string._length);
    }

    StringBase& operator+=(const StringBase& string)
    {
        operator=(StringBase(*this, string, 0, string._length));
        return *this;
    }

    const TChar* RawString()
    {
        if (_length != _realLength)
        {
            TChar* newBuffer = new TChar[_length + 1];
            memcpy(newBuffer, _buffer + _start, _length * sizeof(_buffer[0]));
            newBuffer[_length] = Zero;
            RefDec();
            _buffer = newBuffer;
            _refCount = new size_t(1);
            _start = 0;
            _length = _length;
            _realLength = _length;
        }
        return _buffer;
    }

    TChar operator[](size_t index)
    {
#ifdef PLDEBUG
        CHECK_ERROR(_start + index < _length && index >= 0, "Index out of range");
#endif
        return _buffer[_start + index];
    }

    size_t Length()
    {
        return _length;
    }

    StringBase Trim()
    {
        TChar* start = _buffer + _start;
        TChar* end = _buffer + _start + _length;
        while (*start++ == ' ')
            continue;
        start--;
        while (*--end == ' ')
            continue;
        end++;
        return StringBase(start, end);
    }

    StringBase TrimStart()
    {
        TChar* start = _buffer + _start;
        while (*start++ == ' ')
            continue;
        start--;
        return StringBase(start, _buffer + _start + _length);
    }

    StringBase TrimEnd()
    {
        TChar* end = _buffer + _start + _length;
        while (*--end == ' ')
            continue;
        return StringBase(_buffer + _start, end);
    }

    bool StartsWith(StringBase str)
    {
        if (_length >= str._length)
        {
            return StringBase(*this, _start, str._length) == str;
        }
        return false;
    }

    bool EndsWith(StringBase str)
    {
        if (_length >= str._length)
        {
            return StringBase(*this, _start + _length - str._length, str._length) == str;
        }
        return false;
    }

    StringBase SubStr(size_t start, size_t count)
    {
        CHECK_ERROR(start >= 0 && start < _length && start + count <= _length, "Index out of range.");
        return StringBase(*this, _start + start, count);
    }

    StringBase SubStr(size_t start)
    {
        CHECK_ERROR(start >= 0 && start < _length, "Index out of range.");
        return SubStr(start, _length - start);
    }

    size_t IndexOf(TChar ch, size_t startIndex, size_t count)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length && count > 0 && startIndex + count <= _length, "Index out of range");
        for (size_t i = 0; i < count; i++)
        {
            if (_buffer[_start + startIndex + i] == ch)
            {
                return i;
            }
        }
        return -1;
    }
    size_t IndexOf(TChar ch, size_t start)
    {
        CHECK_ERROR(start >= 0 && start < _length, "Index out of range.");
        return IndexOf(ch, start, _length - start);
    }
    size_t IndexOf(TChar ch)
    {
        return IndexOf(ch, 0, _length);
    }
    size_t LastIndexOf(TChar value, size_t startIndex, size_t count)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length && count > 0 && startIndex + count <= _length, "Index out of range");

        for (size_t i = count; i != 0; i--)
        {
            if (_buffer[_start + startIndex + i - 1] == value)
            {
                return i;
            }
        }
        return -1;
    }
    size_t LastIndexOf(TChar value, size_t startIndex)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length, "Index out of range");

        return LastIndexOf(value, startIndex, _length - startIndex);
    }
    size_t LastIndexOf(TChar value)
    {
        return LastIndexOf(value, 0, _length);
    }
};

using AString = StringBase<char>;
using WString = StringBase<wchar_t>;

}


#endif // !STRING_H

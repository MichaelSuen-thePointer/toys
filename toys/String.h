#pragma once
#ifndef STRING_H
#define STRING_H

#include "Base.h"
#include <memory.h>
namespace pl
{
class AnsiString: public Object
{
public: //public fields
    static const AnsiString Empty;
    static const char Zero = '\0';
private: //private fields
    mutable char* _buffer;
    mutable size_t* _refCount;
    mutable size_t _start;
    mutable size_t _length;
    mutable size_t _realLength;

public: //public member func
    static size_t CStringLength(const char* String)
    {
        size_t size = 0;
        while (*String++)
        {
            size++;
        }
        return size;
    }
    static int Compare(const AnsiString& lhs, const AnsiString& rhs)
    {
        char* lBuf = lhs._buffer + lhs._start;
        char* rBuf = rhs._buffer + rhs._start;
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
    static bool Equal(const AnsiString& lhs, const AnsiString& rhs)
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
        (*_refCount)++;
    }
    void RefDec()
    {
        (*_refCount)--;
        if (_refCount == 0)
        {
            delete _refCount;
            delete[] _buffer;
        }
    }
public: /*ctor*/
    AnsiString()
        : _buffer(new char[1])
        , _refCount(0)
        , _start(0)
        , _length(0)
        , _realLength(1)
    {
        _buffer[0] = '\0';
    }
    AnsiString(const char* string, size_t start, size_t length)
        : _buffer(new char[length + 1])
        , _refCount(new size_t(1))
        , _start(start)
        , _length(length)
        , _realLength(length)
    {
        memcpy(_buffer, string + start, length * sizeof(char));
        _buffer[_length] = '\0';
    }
    AnsiString(const char* string, bool copy = true)
        : _buffer(nullptr)
        , _refCount(new size_t(1))
        , _start(0)
        , _length(CStringLength(string))
        , _realLength(_length)
    {
        if (copy)
        {
            _buffer = new char[_length + 1];
            _buffer[_length] = Zero;
            memcpy(_buffer, string, _length * sizeof(char));
        }
        else
        {
            _buffer = const_cast<char*>(string);
        }
    }
    AnsiString(const char* string, size_t length)
        : AnsiString(string, 0, length)
    {
    }
    AnsiString(const char* start, const char* end)
        : AnsiString(start, 0, end - start)
    {
    }
    AnsiString(char ch)
        : AnsiString(1, ch)
    {
    }
    AnsiString(size_t count, char ch)
        : _buffer(new char[count + 1])
        , _refCount(new size_t(1))
        , _start(0)
        , _length(count)
        , _realLength(count)
    {
        memset(_buffer, ch, _length * sizeof(char));
        _buffer[_length] = '\0';
    }
    AnsiString(const AnsiString& string, size_t start, size_t length)
        : _buffer(string._buffer)
        , _refCount(string._refCount)
        , _start(start)
        , _length(length)
        , _realLength(string._realLength)
    {
        CHECK_ERROR(start + length <= string._length, "Index out of range");
        RefInc();
    }
    AnsiString(const AnsiString& string)
        : AnsiString(string, string._start, string._length)
    {
    }
    AnsiString(AnsiString&& string)
        : _buffer(string._buffer)
        , _refCount(string._refCount)
        , _start(string._start)
        , _length(string._length)
        , _realLength(string._realLength)
    {
        string._buffer = nullptr;
        string._refCount = nullptr;
    }
    AnsiString(const AnsiString& string, size_t start)
        : AnsiString(string, start, string._length - start)
    {
    }
    ~AnsiString()
    {
        if (_refCount)
        {
            RefDec();
        }
    }

    AnsiString& operator=(const AnsiString& string)
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
    AnsiString& operator=(AnsiString&& string)
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
    AnsiString(const AnsiString& concat1, const AnsiString& concat2, int beforeIndex, int length)
        : _buffer(new char[beforeIndex + length + 1])
        , _refCount(new size_t(1))
        , _start(0)
        , _length(beforeIndex + length)
        , _realLength(beforeIndex + length)
    {
        memcpy(_buffer, concat1._buffer + concat1._start, beforeIndex * sizeof(char));
        memcpy(_buffer + beforeIndex, concat2._buffer + concat2._start, length * sizeof(char));
        _buffer[_length] = '\0';
    }
public: //public member func
    bool operator==(const AnsiString& String) { return Equal(*this, String); }
    bool operator!=(const AnsiString& String) { return !Equal(*this, String); }
    bool operator> (const AnsiString& String) { return Compare(*this, String) > 0; }
    bool operator>=(const AnsiString& String) { return Compare(*this, String) >= 0; }
    bool operator< (const AnsiString& String) { return Compare(*this, String) < 0; }
    bool operator<=(const AnsiString& String) { return Compare(*this, String) <= 0; }

    AnsiString operator+(const AnsiString& string)
    {
        return AnsiString(*this, string, _length, string._length);
    }

    AnsiString& operator+=(const AnsiString& string)
    {
        operator=(AnsiString(*this, string, 0, string._length));
        return *this;
    }

    const char* RawString()
    {
        if (_length != _realLength)
        {
            char* newBuffer = new char[_length + 1];
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

    char operator[](size_t index)
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

    AnsiString Trim()
    {
        char* start = _buffer + _start;
        char* end = _buffer + _start + _length;
        while (*start++ == ' ')
            continue;
        start--;
        while (*--end == ' ')
            continue;
        end++;
        return AnsiString(start, end);
    }

    AnsiString TrimStart()
    {
        char* start = _buffer + _start;
        while (*start++ == ' ')
            continue;
        start--;
        return AnsiString(start, _buffer + _start + _length);
    }

    AnsiString TrimEnd()
    {
        char* end = _buffer + _start + _length;
        while (*--end == ' ')
            continue;
        return AnsiString(_buffer + _start, end);
    }

    bool StartsWith(AnsiString str)
    {
        if (_length >= str._length)
        {
            return AnsiString(*this, _start, str._length) == str;
        }
        return false;
    }

    bool EndsWith(AnsiString str)
    {
        if (_length >= str._length)
        {
            return AnsiString(*this, _start + _length - str._length, str._length) == str;
        }
        return false;
    }

    AnsiString SubStr(size_t start, size_t count)
    {
        CHECK_ERROR(start >= 0 && start < _length && start + count <= _length, "Index out of range.");
        return AnsiString(*this, _start + start, count);
    }

    AnsiString SubStr(size_t start)
    {
        CHECK_ERROR(start >= 0 && start < _length, "Index out of range.");
        return SubStr(start, _length - start);
    }

    size_t IndexOf(char ch, size_t startIndex, size_t count)
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
    size_t IndexOf(char ch, size_t start)
    {
        CHECK_ERROR(start >= 0 && start < _length, "Index out of range.");
        return IndexOf(ch, start, _length - start);
    }
    size_t IndexOf(char ch)
    {
        return IndexOf(ch, 0, _length);
    }
    size_t IndexOf(const AnsiString& value, size_t startIndex, size_t count)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length && count > 0 && startIndex + count <= _length, "Index out of range");
        if (value._length == 1)
        {
            return IndexOf(value._buffer[value._start], startIndex, count);
        }
        if (value._length < count)
        {
            return -1;
        }
        size_t findStrLength = value._length;
        size_t length = _start + count;
        char* string = value._buffer + value._start;
        size_t subStrOffset = 1;
        size_t partialLength = 0;
        size_t* partialMatchTable = new size_t[findStrLength + 1];
        partialMatchTable++;
        partialMatchTable[-1] = -1;
        partialMatchTable[0] = 0;
        for (size_t i = 1; i < findStrLength; i++)
        {
            if (string[i] != string[i - subStrOffset])
            {
                subStrOffset = i;
                partialLength = 0;
            }
            else
            {
                partialLength++;
            }
            partialMatchTable[i] = partialLength;
        }
        /*matchedLength == i*/
        size_t index = _start + startIndex;
        size_t indexMax = length - findStrLength + 1;
        size_t i;
        for (i = 0; i < findStrLength && index < indexMax; i++)
        {
            if (_buffer[index + i] != string[i])
            {
                int offset = i - partialMatchTable[i - 1];
                index += offset;
                i -= offset;
            }
        }
        delete[](partialMatchTable - 1);
        if (i == findStrLength)
        {
            return index;
        }
        else
        {
            return -1;
        }
    }
    size_t IndexOf(const AnsiString& findStr, size_t startIndex)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length, "Index out of range");
        return IndexOf(findStr, startIndex, _length - startIndex);
    }
    size_t IndexOf(const AnsiString& findStr)
    {
        return IndexOf(findStr, 0, _length);
    }
    size_t LastIndexOf(char value, size_t startIndex, size_t count)
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
    size_t LastIndexOf(char value, size_t startIndex)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length, "Index out of range");

        return LastIndexOf(value, startIndex, _length - startIndex);
    }
    size_t LastIndexOf(char value)
    {
        return LastIndexOf(value, 0, _length);
    }
    size_t LastIndexOf(const AnsiString& value, size_t startIndex, size_t count)
    {
        CHECK_ERROR(startIndex >= 0 && startIndex < _length && count > 0 && startIndex + count <= _length, "Index out of range");
        if (value._length == 1)
        {
            return LastIndexOf(value._buffer[value._start], startIndex, count);
        }
        if (value._length < count)
        {
            return -1;
        }
        size_t findStrLength = value._length;
        size_t length = _start + count;
        char* string = value._buffer + value._start;
        size_t subStrOffset = 1;
        size_t partialLength = 0;
        size_t* partialMatchTable = new size_t[findStrLength + 1];
        partialMatchTable[findStrLength] = -1;
        partialMatchTable[findStrLength - 1] = 0;
        for (size_t i = findStrLength - 2; i != (size_t)-1; i--)
        {
            if (string[i] != string[i + subStrOffset])
            {
                subStrOffset = findStrLength - i;
                partialLength = 0;
            }
            else
            {
                partialLength++;
            }
            partialMatchTable[i] = partialLength;
        }
        /*matchedLength == findStrLength - 1 - i*/
        size_t index = _start + startIndex + count - 1;
        size_t indexMin = _start + startIndex + findStrLength - 1;
        size_t i;
        for (i = findStrLength - 1; i != (size_t)-1 && index >= indexMin; i--)
        {
            if (_buffer[index - (findStrLength - 1 - i)] != string[i])
            {
                int offset = findStrLength - 1 - i - partialMatchTable[i + 1];
                index -= offset;
                i += offset;
            }
        }
        delete[] partialMatchTable;
        if (i == (size_t)-1)
        {
            return index - findStrLength + 1;
        }
        else
        {
            return -1;
        }
    }
    size_t LastIndexOf(const AnsiString& value, size_t startIndex)
    {
        return LastIndexOf(value, startIndex, _length);
    }
    size_t LastIndexOf(const AnsiString& value)
    {
        return LastIndexOf(value, 0, _length);
    }


};

}


#endif // !STRING_H

#pragma once
#include "ScopeGuard.h"
#include <cstring>
#include <cassert>
#include <string>
#include <iterator>

namespace mq
{

template<class TEnc>
class string
{
public:
    using value_type = typename TEnc::char_type;
    using traits_type = std::char_traits<value_type>;
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using iterator = value_type*;
    using const_iterator = const value_type*;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    constexpr static size_t npos = static_cast<size_t>(-1);

    string(string const& right)
    {
        assign_right(right);
    }

    string& operator=(string const& right)
    {
        assign_right(right);
        return *this;
    }

    string(string&& right) noexcept
    {
        assign_right(std::move(right));
    }

    string& operator=(string&& right) noexcept
    {
        assign_right(std::move(right));
        return *this;
    }

    constexpr string()
    {
        _sso._str[0] = value_type();
        _size = 0;
        _capacity = _ssosize - 1;
    }

    string(size_type count, value_type ch)
    {
        if (count >= _ssosize)
        {
            auto newPtr = new value_type[count + 1];
            MQ_SCOPE_FAIL{ delete[] newPtr; };
            traits_type::assign(newPtr, count, ch);
            newPtr[count] = value_type();
            _sso._ptr = newPtr;
            _capacity = count;
        }
        else
        {
            traits_type::assign(_sso._str, count, ch);
            _sso._str[count] = 0;
            _capacity = _ssosize;
        }
        _size = count;
    }

    constexpr string(value_type const* str)
    {
        size_t size = strlen(str);
        if (size >= _ssosize)
        {
            auto newPtr = new value_type[size + 1];
            MQ_SCOPE_FAIL{ delete[] newPtr; };
            memcpy(newPtr, str, size + 1);
            _sso._ptr = newPtr;
            _capacity = size;
        }
        else
        {
            memcpy(_sso._str, str, size + 1);
            _capacity = _ssosize;
        }
        _size = size;
    }

    value_type operator[](size_t i) const
    {
        return const_cast<string&>(*this)[i];
    }

    value_type& operator[](size_t i)
    {
        assert(i <= _size);
        return buffer()[i];
    }

    void push_back(value_type ch)
    {
        if (_size == _capacity)
        {
            auto newSize = _size + _size / 2;
            newSize = newSize == _size ? _size + 1 : newSize;
            reserve(newSize);
        }
        auto buf = buffer();
        buf[_size] = ch;
        buf[_size + 1] = value_type();
        ++_size;
    }

    void swap(string& right) noexcept
    {
        if (right._size >= _ssosize)
        { //hopefully the redundant if will be optimized
            assign_right(std::move(right));
        }
        else
        {
            std::swap(_sso._str, right._sso._str);
            std::swap(_size, right._size);
            std::swap(_capacity, right._capacity);
        }
    }

    ~string()
    {
        tidy();
    }

private:
    void reserve(size_t newCap)
    {
        if (newCap < _size)
        {
            shrink_to_fit();
            return;
        }
        if (newCap > _capacity || newCap >= _ssosize)
        {
            assert(newCap >= _ssosize);
            auto newPtr = new value_type[newCap + 1];
            MQ_SCOPE_FAIL{ delete[] newPtr; };
            memcpy(newPtr, buffer(), _size + 1);
            tidy();
            _sso._ptr = newPtr;
            _capacity = newCap;
        }
    }

    void shrink_to_fit()
    {
        if (!is_sso() && _size < _capacity)
        {
            auto newPtr = new value_type[_size + 1];
            MQ_SCOPE_FAIL{ delete[] newPtr; };
            memcpy(newPtr, buffer(), _size + 1);
            tidy();
            _sso._ptr = newPtr;
            _capacity = _size;
        }
    }

    value_type* buffer()
    {
        if (is_sso())
        {
            return _sso._str;
        }
        return _sso._ptr;
    }

    bool is_sso() const
    {
        return _size >= _ssosize;
    }

    void assign_right(string&& right) noexcept
    {
        if (is_sso())
        {
            std::swap(right._sso._ptr, this->_sso._ptr);
            std::swap(right._size, this->_size);
            std::swap(right._capacity, this->_capacity);
        }
        else
        {
            memcpy(_sso._str, right._sso._str, _ssosize);
            _size = right._size;
            _capacity = right._capacity;
        }
    }
    void assign_right(string const& right)
    {
        if (is_sso())
        {
            value_type* newPtr = new value_type[right._capacity];
            MQ_SCOPE_FAIL{ delete[] newPtr; };
            memcpy(newPtr, right._sso._ptr, right._capacity);
            tidy();
            _sso._ptr = newPtr;
        }
        else
        {
            tidy();
            memcpy(_sso._str, right._sso._str, _ssosize);
        }
        _size = right._size;
        _capacity = right._capacity;
    }
    void tidy()
    {
        if (!is_sso())
        {
            delete[] _sso._ptr;
        }
    }

    constexpr static size_t strlen(value_type const* s) noexcept
    {
        size_t len = 0;
        while (*s++)
        {
            ++len;
        }
        return len;
    }

    constexpr static size_t _ssosize = sizeof(value_type) > 16 ? 1 : 16 / sizeof(value_type);
    union _sso
    {
        pointer _ptr;
        value_type _str[_ssosize];
        char _alias[sizeof(_str)];
    } _sso;
    size_t _size;
    size_t _capacity;
};


struct utf32_encoding
{
    using char_type = char32_t;
    constexpr static bool is_multi = false;
    struct state {};
};

struct utf16_encoding
{
    using char_type = char16_t;
    constexpr static bool is_multi = true;
    struct state
    {
        friend struct utf16_encoding;
    private:
        char16_t prev = 0;
    };
};

struct utf8_encoding
{
    using char_type = char;
    constexpr static bool is_multi = true;

    struct state
    {
        friend struct utf8_encoding;
    private:
        int cnt = 0;
        char32_t ch = 0;
    };

    static bool validate_string(string<utf8_encoding> str);
    static char_type* next_codepoint(char_type* str);
    static string<utf32_encoding> to_utf32(char_type* str);

    template<class T>
    static typename T::char_type* convert_to(char_type* str);
};

}

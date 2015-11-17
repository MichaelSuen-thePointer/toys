#pragma once
#ifndef NUMBERS_H
#define NUMBERS_H
#include "Base.h"
#include "Template.h"
#include <vector>
#include <string>
#include "StringUtils.h"

namespace pl
{
namespace number
{
struct Zero
{};

template <typename TPrev>
struct PlusOne
{
    using Type = typename PlusOne<TPrev>;
    using Prev = typename TPrev;
};

template <typename T>
struct ShowValue
{
    enum: unsigned { Value = ShowValue<T::Prev>::Value + 1 };
};

template <>
struct ShowValue<Zero>
{
    enum: unsigned { Value = 0 };
};

template <unsigned Num>
struct Integer
{
    using Type = typename PlusOne<typename Integer<Num - 1>::Type>::Type;
};

template <>
struct Integer<0>
{
    using Type = Zero;
};

template <unsigned Num1, unsigned Num2>
struct AddNum
{
    using Type = typename Integer<Num1 + Num2>::Type;
};

template <typename NumberT1, typename NumberT2>
struct Add
{
    using Type = typename AddNum<ShowValue<typename NumberT1::Type>::Value, ShowValue<typename NumberT2::Type>::Value>::Type;
};



} //namespace number

class ParseError: public std::runtime_error
{
public:
    ParseError(int position, char ch)
        : runtime_error(StringFormat("Unexpected char '", ch, "' in position ", position))
    {
    }
    ParseError(const char* msg)
        : runtime_error(msg)
    {
    }
    ParseError()
        : runtime_error("Empty string")
    {
    }
};

class CalculationError: public std::logic_error
{
public:
    CalculationError(const char* msg)
        : logic_error(msg)
    {
    }
};
class Number
{
public:
    enum NumberType
    {
        SignedInteger,
        UnsignedInteger,
        Float
    };
    enum Radix
    {
        r2, r8, r10, r16
    };
protected:
    std::vector<bool> _num;
    NumberType _type;

    static std::vector<bool> Parse(const char* strNumber);
    static Radix Preprocess(std::string& strNumber);
    static void Map2(std::string& strNumber, std::vector<bool>& dest);
    static void Map8(std::string& strNumber, std::vector<bool>& dest);
    static void Map10(std::string& strNumber, std::vector<bool>& dest);
    static void Map16(std::string& strNumber, std::vector<bool>& dest);

    template<typename TInteger>
    static std::vector<bool> ToVectorBool(TInteger intNumber, State<0>);

    template<typename TFloat>
    static std::vector<bool> ToVectorBool(TFloat floatNumber, State<1>);

    template<typename TCStr>
    static std::vector<bool> ToVectorBool(TCStr cstrNumber, State<2>);

    template<typename TOther>
    static std::vector<bool> ToVectorBool(TOther other, FalseType);

public:
    Number(char* cstrNumber, NumberType type = SignedInteger);

    Number(std::vector<bool>& bitString, NumberType type = SignedInteger);
    Number(std::vector<bool>&& bitString, NumberType type = SignedInteger);

    template<typename TNumber>
    Number(TNumber number);

    Number(const Number& number);
    Number(Number&& number);

    Number operator+(void);
    Number operator-(void);
    Number operator+(const Number& rhs);

    void SignedExpandTo(size_t size);
    void ZeroExpandTo(size_t size);
    void ExpandTo(size_t size);

    void SignedExpand(size_t size);
    void ZeroExpand(size_t size);
    void Expand(size_t size);

    bool SignBit() const;

    bool IsUnsigned() const;
    friend std::ostream& operator<<(std::ostream& os, Number& number);
};

Number::Number(char* cstrNumber, Number::NumberType type/*=SignedInteger*/)
    : _num(RvalueCast(Parse(cstrNumber)))
    , _type(SignedInteger)
{
    if (cstrNumber[0] == '-')
    {
        this->operator-();
    }
}
Number::Number(std::vector<bool>& bitString, NumberType type/* = SignedInteger*/)
    : _num(bitString)
    , _type(type)
{
}
Number::Number(std::vector<bool>&& bitString, NumberType type/* = SignedInteger*/)
    : _num(RvalueCast(bitString))
    , _type(type)
{
}

template<typename TNumber>
Number::Number(TNumber number)
    : _num(RvalueCast(ToVectorBool(number, MultiState<TNumber, IsInteger, IsFloat, IsCString>::Result())))
    , _type(IsInteger<TNumber>::Result::Value ? SignedInteger : Float)
{
}

template<typename TInteger>
static std::vector<bool> Number::ToVectorBool(TInteger intNumber, State<0>)
{
    //Integer
    std::vector<bool> result(sizeof(TInteger) * 8);
    for (int i = 0; i < sizeof(TInteger) * 8; i++)
    {
        result[i] = (intNumber >> i) & 0x1;
    }
    return RvalueCast(result);
}

template<typename TFloat>
static std::vector<bool> Number::ToVectorBool(TFloat floatNumber, State<1>)
{
    //Float
    static_assert(0, "Not Implement yet");
}

template<typename TCStr>
static std::vector<bool> Number::ToVectorBool(TCStr cstrNumber, State<2>)
{
    //char*
    return RvalueCast(Parse(cstrNumber));
}


template<typename TOther>
static std::vector<bool> Number::ToVectorBool(TOther other, FalseType)
{
    static_assert(0, "Not a number.");
}

Number::Number(const Number& number)
    : _num(number._num)
    , _type(number._type)
{
}
Number::Number(Number&& number)
    : _num(PerfectForward<std::vector<bool>>(number._num))
    , _type(number._type)
{
}

std::vector<bool> Number::Parse(const char* cstrNumber)
{
    std::vector<bool> result;
    std::string strNumber = cstrNumber;
    Radix radix = Preprocess(strNumber);
    switch (radix)
    {
    case r2:
    {
        result.reserve(strNumber.length());
        Map2(strNumber, result);
        break;
    }
    case r8:
    {
        result.reserve(strNumber.length() * 3);
        Map8(strNumber, result);
        break;
    }
    case r10:
    {
        result.reserve(strNumber.length() * 4);
        Map10(strNumber, result);
        break;
    }
    case r16:
    {
        result.reserve(strNumber.length() * 4);
        Map16(strNumber, result);
        break;
    }
    }
    return result;
}

void Number::Map2(std::string& strNumber, std::vector<bool>& dest)
{
    if (sizeof(bool) == sizeof(char))
    {
        std::reverse(strNumber.begin(), strNumber.end());
        std::initializer_list<bool> list((bool*)(&strNumber[0]), (bool*)(&strNumber[strNumber.length()]));
        dest.assign(list);
    }
    else
    {
        size_t length = strNumber.length();
        for (size_t i = length - 1; i >= length; i++)
        {
            dest[i] = strNumber[i] == 1;
        }
    }
}

void Number::Map8(std::string& strNumber, std::vector<bool>& dest)
{
#define F false
#define T true
    static const std::initializer_list<bool> r8tor2[] =
    {
        {F,F,F},{T,F,F},{F,T,F},{T,T,F},
        {F,F,T},{T,F,T},{F,T,T},{T,T,T}
    };
#undef F
#undef T
    for (auto iter = strNumber.rbegin(); iter != strNumber.rend(); iter++)
    {
        dest.insert(dest.end(), r8tor2[*iter]);
    }
}

void Number::Map10(std::string& strNumber, std::vector<bool>& dest)
{
    std::string::iterator ch = strNumber.begin();
    int rest = 0;
    int num;
    int index = 0;
    for (auto& elem : strNumber)
    {
        elem -= '0';
    }
    for (;;)
    {
        while (ch != strNumber.end() && *ch == 0)
        {
            ch++;
        }
        if (ch == strNumber.end())
        {
            break;
        }
        while (ch != strNumber.end())
        {
            num = *ch + rest * 10;
            rest = num & 1; //num % 2;
            *ch = num >> 1; //num / 2;
            ch++;
        }
        ch = strNumber.begin();
        dest[index++] = rest == 1;
        rest = 0;
    }
}

void Number::Map16(std::string& strNumber, std::vector<bool>& dest)
{
#define F false
#define T true
    static const std::initializer_list<bool> r16tor2[] =
    {
        {F,F,F,F}, {T,F,F,F}, {F,T,F,F}, {T,T,F,F},
        {F,F,T,F}, {T,F,T,F}, {F,T,T,F}, {T,T,T,F},
        {F,F,F,T}, {T,F,F,T}, {F,T,F,T}, {T,T,F,T},
        {F,F,T,T}, {T,F,T,T}, {T,T,T,F}, {T,T,T,T}
    };
#undef F
#undef T
    for (auto iter = strNumber.rbegin(); iter != strNumber.rend(); iter++)
    {
        dest.insert(dest.end(), r16tor2[*iter]);
    }

}

Number::Radix Number::Preprocess(std::string& strNumber)
{
    if (strNumber.length() == 0)
    {
        throw ParseError();
    }
    if (strNumber[0] == '0')
    {
        if (strNumber[1] == 'x')
        {
            if (strNumber.length() == 2)
            {
                throw ParseError("Empty after 0x");
            }
            strNumber.erase(strNumber.begin(), strNumber.begin() + 2);
            for (auto iter = strNumber.begin(); iter != strNumber.end(); iter++)
            {
                *iter = toupper(*iter);
                if (!isdigit(*iter) && !(*iter >= 'A' && *iter <= 'F'))
                {
                    throw ParseError(iter - strNumber.begin(), *iter);
                }
                if (*iter >= 'A' && *iter <= 'F')
                {
                    *iter = *iter - 'A' + 10;
                }
                else
                {
                    *iter -= '0';
                }
            }
            return r16;
        }
        else //r8
        {
            strNumber.erase(strNumber.begin(), strNumber.begin() + 1);
            for (auto iter = strNumber.begin(); iter != strNumber.end(); iter++)
            {
                if (*iter > '7' || *iter < '0')
                {
                    throw ParseError(iter - strNumber.begin(), *iter);
                }
                *iter -= '0';
            }
            return r8;
        }
    }
    else if (strNumber[0] == 'b' || strNumber[0] == 'B')
    {
        if (strNumber.length() == 1)
        {
            throw ParseError("Empty after B");
        }
        strNumber.erase(strNumber.begin(), strNumber.begin() + 1);
        for (auto iter = strNumber.begin(); iter != strNumber.end(); iter++)
        {
            if (*iter != '0' && *iter != '1')
            {
                throw ParseError(iter - strNumber.begin(), *iter);
            }
            *iter -= '0';
        }
        return r2;
    }
    else
    {
        auto iter = strNumber.begin();
        if (strNumber[0] == '-')
        {
            if (strNumber.length() == 1)
            {
                throw ParseError("Empty after -");
            }
            iter++;
        }
        for (; iter != strNumber.end(); iter++)
        {
            if (!isdigit(*iter))
            {
                throw ParseError(iter - strNumber.begin(), *iter);
            }
            *iter -= '0';
        }
        return r10;
    }
}

void Number::SignedExpandTo(size_t size)
{
    size_t oldSize = this->_num.size();
    size_t deltaSize = size - oldSize;
    bool signBit = _num[_num.size() - 1];
    _num.insert(_num.end(), deltaSize, signBit);
}

void Number::ZeroExpandTo(size_t size)
{
    size_t oldSize = this->_num.size();
    size_t deltaSize = size - oldSize;
    _num.insert(_num.end(), deltaSize, false);
}

void Number::ExpandTo(size_t size)
{
    if (_type == SignedInteger)
    {
        SignedExpandTo(size);
    }
    else if (_type == UnsignedInteger)
    {
        ZeroExpandTo(size);
    }
    else
    {
        throw CalculationError("Cannot expand a floating point.");
    }
}

void Number::SignedExpand(size_t size)
{
    bool signBit = _num[_num.size() - 1];
    _num.insert(_num.end(), size, signBit);
}

void Number::ZeroExpand(size_t size)
{
    _num.insert(_num.end(), size, false);
}

void Number::Expand(size_t size)
{
    if (_type == SignedInteger)
    {
        SignedExpand(size);
    }
    else if (_type == UnsignedInteger)
    {
        ZeroExpand(size);
    }
    else
    {
        throw CalculationError("Cannot expand a floating point.");
    }
}

bool Number::SignBit() const
{
    if (this->_type == UnsignedInteger)
    {
        throw CalculationError("Number is unsigned.");
    }
    else if (_type == SignedInteger)
    {
        return this->_num[this->_num.size() - 1];
    }
    else
    {
        FAST_FAIL;
    }
}

bool Number::IsUnsigned() const
{
    return this->_type == UnsignedInteger;
}


Number Number::operator+(void)
{
    return Number(*this);
}

Number Number::operator-(void)
{
    Number result(*this);
    bool metOne = false;
    for (auto& bit : result._num)
    {
        bit = bit ^ metOne;
        if (bit == true && !metOne)
        {
            metOne = true;
        }
    }
    return result;
}

Number Number::operator+(const Number& rhs)
{
    size_t maxSize = Max(this->_num.size(), rhs._num.size());
    Number a(*this);
    Number b(rhs);
    std::vector<bool> result(maxSize);
    a.ExpandTo(maxSize);
    b.ExpandTo(maxSize);
    if (a._type != Float && b._type != Float && a._type == b._type)
    {
        size_t i;
        bool ci = false; //carry
        for (i = 0; i < maxSize; i++)
        {
            bool ai = a._num[i];
            bool bi = b._num[i];
            result[i] = (ai ^ bi ^ ci) == 1;
            ci = (ai & bi | bi & ci | ci & ai) == 1;
        }
        if (a.IsUnsigned() && ci == true)
        {
            result.insert(result.end(), 1, ci);
        }
        else if (a.SignBit() == b.SignBit() && a.SignBit() != *(result.rbegin()))
        {
            result.insert(result.end(), 1, a.SignBit());
        }
        return Number(RvalueCast(result), a._type);
    }
    else
    {
        throw CalculationError("Wrong type of two operands in operator+");
    }
}

std::ostream& operator<<(std::ostream& os, Number& number)
{
    for (auto iter = number._num.rbegin(); iter != number._num.rend(); iter++)
    {
        os << *iter;
    }
    return os;
}


} //namespace pl


#endif // !NUMBERS_H

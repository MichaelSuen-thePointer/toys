#pragma once
#include <type_traits>
namespace mq
{

template<char c>
struct char_constant : std::integral_constant<char, c>
{
};


template<class T, T c1, T c2>
constexpr std::bool_constant<c1 == c2> operator==(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}


template<class T, T c1, T c2>
constexpr std::bool_constant<c1 != c2> operator!=(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}

template<class T, T c1, T c2>
constexpr std::bool_constant<(c1 > c2)> operator>(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}

template<class T, T c1, T c2>
constexpr std::bool_constant<(c1 >= c2)> operator>=(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}

template<class T, T c1, T c2>
constexpr std::bool_constant<(c1 < c2)> operator<(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}

template<class T, T c1, T c2>
constexpr std::bool_constant<(c1 <= c2)> operator<=(std::integral_constant<T, c1>, std::integral_constant<T, c2>)
{
    return{};
}

template<bool b1, bool b2>
constexpr std::bool_constant<b1 && b2> operator&&(std::bool_constant<b1>, std::bool_constant<b2>)
{
    return{};
}

template<bool b1, bool b2>
constexpr std::bool_constant<b1 || b2> operator||(std::bool_constant<b1>, std::bool_constant<b2>)
{
    return{};
}

template<bool v, class T1, class T2>
constexpr decltype(auto) cond(std::bool_constant<v>, T1 a, T2 b)
{
    if constexpr (v)
    {
        return a;
    }
    else
    {
        return b;
    }
}

template<class Curr, class Cond, class Iter>
constexpr decltype(auto) iter(Curr i, Cond c, Iter e)
{
    //static_assert(c(i).value);
    if constexpr (c(i).value)
    {
        return iter(e(i), c, e);
    }
    else
    {
        return i;
    }
}

} //namespace mq
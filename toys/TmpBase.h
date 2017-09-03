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

struct fallthrough_t {} fallthrough;
struct default_t {} conds_default{};


constexpr std::true_type operator==(default_t, default_t)
{
    return {};
}

template<class T>
constexpr std::false_type operator==(default_t, T)
{
    return {};
}

template<class T>
constexpr std::false_type operator==(T, default_t)
{
    return {};
}

enum class cond_reduce_state
{
    accept_case,
    reject_case,
    accept_body,
    reject_body,
    result,
    default_case,
    default_result,
};

template<class T>
using false_t = std::false_type;

template<class Val, cond_reduce_state state = cond_reduce_state::accept_case, class R = void>
struct conds_reduce
{
    constexpr std::add_rvalue_reference_t<R> result()
    {
        return std::move(*static_cast<R*>(nullptr));
    }

    template<class T>
    constexpr decltype(auto) operator>>(T v)
    {
        if constexpr (state == cond_reduce_state::accept_case)
        {
            if constexpr ((Val{} == v).value)
            {
                return conds_reduce<Val, cond_reduce_state::accept_body, R>{};
            }
            else if constexpr (std::is_same_v<T, default_t>)
            {
                return conds_reduce<Val, cond_reduce_state::default_case, R>{};
            }
            else
            {
                return conds_reduce<Val, cond_reduce_state::reject_body, R>{};
            }
        }
        else if constexpr (state == cond_reduce_state::reject_body)
        {
            return conds_reduce<Val, cond_reduce_state::accept_case, R>{};
        }
        else if constexpr (state == cond_reduce_state::accept_body)
        {
            if constexpr (std::is_same_v<T, fallthrough_t>)
            {
                return conds_reduce<Val, cond_reduce_state::reject_case, R>{};
            }
            else
            {
                return conds_reduce<Val, cond_reduce_state::result, T>{};
            }
        }
        else if constexpr (state == cond_reduce_state::reject_case)
        {
            return conds_reduce<Val, cond_reduce_state::accept_body, R>{};
        }
        else if constexpr (state == cond_reduce_state::result)
        {
            return conds_reduce<Val, cond_reduce_state::result, R>{};
        }
        else if constexpr (state == cond_reduce_state::default_case)
        {
            return conds_reduce<Val, cond_reduce_state::default_result, T>{};
        }
        else if constexpr (state == cond_reduce_state::default_result)
        {
            static_assert(false_t<T>{}, "default must be the last case");
        }
        else
        {
            static_assert(false_t<T>{}, "bug");
        }
    }
};

template<class Val, class... CaseBodies>
constexpr decltype(auto) conds(Val v, CaseBodies... r)
{
    static_assert(sizeof...(CaseBodies) % 2 == 0, "should be in pair");

    return (conds_reduce<Val>{} >> ... >> r).result();
}

} //namespace mq
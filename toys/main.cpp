#include <type_traits>
#include <utility>
#include <tuple>
#include <iostream>
#include <cxxabi.h>
#include "TmpBase.h"
#include <vector>

std::ostream& operator<<(std::ostream& os, const std::type_info& nfo)
{
    int status;
    auto realname = abi::__cxa_demangle(nfo.name(), 0, 0, &status);
    os << realname;
    free(realname);
    return os;
}

template<class T>
constexpr static bool always_false = false;

struct _placeholder {} _;

namespace mq
{

template<char c>
constexpr static auto cc = char_constant<c>{};

template<char... chars>
struct char_sequence
{
    template<size_t i>
    constexpr static decltype(auto) get()
    {
        static_assert(i < sizeof...(chars), "internal error");
        return char_constant<std::get<i>(std::make_tuple(chars...))>{};
    }
};

template<class Sequence, size_t _i, class Result>
struct parse_result
{
    constexpr static decltype(auto) sequence()
    {
        return Sequence{};
    }

    constexpr static decltype(auto) get()
    {
        return Sequence::template get<_i>();
    }

    constexpr static decltype(auto) peek()
    {
        return Sequence::template get<_i + 1>();
    }

    constexpr static decltype(auto) result()
    {
        return Result{};
    }

    constexpr static decltype(auto) forward()
    {
        return parse_result<Sequence, _i + 1, Result>{};
    }

    template<class R>
    constexpr static decltype(auto) result(R)
    {
        return parse_result<Sequence, _i, R>{};
    }
};

template<class Derived>
struct regex
{
};

template<char c>
struct match : regex<match<c>>
{
};

template<char c>
constexpr decltype(auto) mkmatch(char_constant<c>)
{
    return match<c>{};
}

template<class T>
struct kleene : regex<kleene<T>>
{
};

template<class T>
constexpr decltype(auto) mkkleene(T)
{
    return kleene<T>{};
}

template<class... Regexes>
struct concat : regex<concat<Regexes...>>
{
};

template<class... Ts>
constexpr decltype(auto) mkconcat(regex<Ts>...)
{
    return concat<Ts...>{};
}

template<class... Rs, class... Ts>
constexpr decltype(auto) mkconcat(concat<Rs...>, regex<Ts>...)
{
    return concat<Rs..., Ts...>{};
}

template<class... Regexes>
struct alter : regex<alter<Regexes...>>
{
};

template<class... Ts>
constexpr decltype(auto) mkalter(regex<Ts>...)
{
    return alter<Ts...>{};
}

template<class... Rs, class... Ts>
constexpr decltype(auto) mkalter(alter<Rs...>, regex<Ts>...)
{
    return alter<Rs..., Ts...>{};
}

struct regex_parser
{
    template<class Seq>
    constexpr static decltype(auto) parse(Seq s)
    {
        return parse_alternative(parse_result<Seq, 0, void>{});
    }
private:
    template<class ParseResult>
    constexpr static decltype(auto) parse_alternative(ParseResult r)
    {
        return iter(parse_concatination(r),
            [](auto res)
        {
            return res.get() == cc<'|'>;
        },
            [](auto res)
        {
            static_assert((res.get() == cc<'|'>).value);
            auto e = parse_concatination(res.forward());
            return e.result(mkalter(res.result(), e.result()));
        });
    }

    template<class ParseResult>
    constexpr static decltype(auto) parse_concatination(ParseResult r)
    {
        return iter(parse_kleene(r),
            [](auto res)
        {
            return (res.get() != cc<'\0'>) && (res.get() != cc<'|'>) && (res.get() != cc<')'>);
        },
            [](auto res)
        {
            auto e = parse_kleene(res);
            return e.result(mkconcat(res.result(), e.result()));
        });
        /* Ïàµ±ÓÚ
        auto regex = mkconcat(parse_kleene(r));
        for (;;)
        {
            if (r.get() £¡= '\0' && r.get() != '|')
            {
                regex = mkconcat(regex, parse_kleene(r.forward()));
            }
            else
            {
                return regex;
            }
        }
        */
    }

    template<class ParseResult>
    constexpr static decltype(auto) parse_kleene(ParseResult r)
    {
        auto matchChar = parse_char(r);

        return cond(matchChar.get() == cc<'*'>, [=](auto _)
        {
            return matchChar.forward().result(mkkleene(matchChar.result()));
        }, [=] (auto _)
        {
            return matchChar;
        })(_);
    }

    template<class ParseResult>
    constexpr static decltype(auto) parse_char(ParseResult r)
    {
        auto token = r.get();

        return conds(token,
            cc<'\\'>, [=] (auto ch)
        {
            //static_assert(always_false<ParseResult>);
            auto f = r.forward();
            return f.forward().result(mkmatch(f.get()));
        }, cc<'('>, [=] (auto ch)
        {
            //static_assert(always_false<ParseResult>);
            auto f = r.forward();
            auto exp = parse_alternative(f);
            static_assert(exp.get() == cc<')'>);
            return exp.forward();
        }, conds_default, [=](auto token)
        {
            return r.forward().result(mkmatch(token));
        })(token);
    }

};

template<class TChar, TChar... chars>
constexpr decltype(auto) operator"" _regex()
{
    return regex_parser::parse(char_sequence<chars..., '\0'>{}).result();
}

}

using namespace mq;

int main()
{
    auto rgx = "a(b|c)*"_regex;

    std::cout << typeid(rgx);
}

#include <type_traits>
#include <utility>
#include <tuple>
#include <iostream>
#include <cxxabi.h>
#include "TmpBase.h"

std::ostream& operator<<(std::ostream& os, const std::type_info& nfo)
{
    int status;
    auto realname = abi::__cxa_demangle(nfo.name(), 0, 0, &status);
    os << realname;
    free(realname);
    return os;
}

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

template<char c>
struct kleene : regex<kleene<c>>
{
};

template<char c>
constexpr decltype(auto) mkkleene(char_constant<c>)
{
    return kleene<c>{};
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
    //private:
    template<class ParseResult>
    constexpr static decltype(auto) parse_alternative(ParseResult r)
    {
        return iter(parse_concatination(r),
            [](auto res)
        {
            //static_assert(res.get() == cc<'|'> || res.get() == cc<'\0'>);
            //static_assert(res.get() == cc<'\0'>);
            return res.get() != cc<'\0'>;
        },
            [](auto res)
        {
            //std::cout << typeid(res) << "\n";
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
            //static_assert(res.get() == cc<'\0'>);
            return (res.get() != cc<'\0'>) && (res.get() != cc<'|'>);
        },
            [](auto res)
        {
            auto e = parse_kleene(res);
            return e.result(mkconcat(res.result(), e.result()));
        });
        /*
        auto regex = mkconcat(parse_kleene(r));
        for (;;)
        {
            if (r.peek() < 'a' || r.peek() > 'z')
            {
                return regex;
            }
            regex = mkconcat(regex, parse_kleene(r.forward()));
        }
        */
    }

    template<class ParseResult>
    constexpr static decltype(auto) parse_kleene(ParseResult r)
    {
        auto token = r.get();
        auto next = r.peek();
        return cond(next == cc<'*'>,
            [=] { return r.forward().forward().result(mkkleene(token)); },
            [=] { return r.forward().result(mkmatch(token)); })();
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
    auto a = "ab*|c"_regex;
    std::cout << typeid(a) << "\n";
}

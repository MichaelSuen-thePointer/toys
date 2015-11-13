#pragma once
#ifndef STRING_UTILS
#define STRING_UTILS
#include <string>

namespace pl
{

template<class... TArgs>
std::string StringFormat(TArgs... rest)
{
    return _StringFormat(rest...);
}

template<class T, class... TArgs>
std::string _StringFormat(T elem, TArgs... rest)
{
    return std::to_string(elem) + _StringFormat(rest...);
}

template<class... TArgs>
std::string _StringFormat(std::string elem, TArgs... rest)
{
    return elem + _StringFormat(rest...);
}

template<class... TArgs>
std::string _StringFormat(const char* elem, TArgs... rest)
{
    return std::string(elem) + _StringFormat(rest...);
}

template<class... TArgs>
std::string _StringFormat(char elem, TArgs... rest)
{
    return std::string(1, elem) + _StringFormat(rest...);
}

template<class T>
std::string _StringFormat(T elem)
{
    return std::to_string(elem);
}

template<>
std::string _StringFormat(const char* elem)
{
    return std::string(elem);
}

template<>
std::string _StringFormat(char elem)
{
    return std::string(1, elem);
}

template<>
std::string _StringFormat(std::string elem)
{
    return elem;
}

}

#endif // !STRING_UTILS

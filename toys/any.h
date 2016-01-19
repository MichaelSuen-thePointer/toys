#pragma once
#ifndef ANY_H
#define ANY_H

#include <type_traits>

class Any
{
protected:
    class ContentBase
    {
    public:
        virtual ~ContentBase() {}
        virtual ContentBase* Clone() const = 0;
    };

    template<typename T>
    class Content: public ContentBase
    {
    protected:
        T _value;
    public:
        Content(const T& value)
            : _value(value)
        {
        }
        Content(T&& value)
            : _value(std::move(value))
        {
        }
        Content<T>* Clone() const override
        {
            return new Content<T>(_value);
        }
    };

    ContentBase* _content;

public:
    Any() noexcept
        : _content(nullptr)
    {
    }
    template<typename T>
    Any(const T& value)
        : _content(new Content<
                   typename std::remove_cv<
                   typename std::decay<T>::type>::type>(value))
    {
    }
    //利用SFINAE,不对Any&类型和const T类型移动构造
    template<typename T>
    Any(T&& value
        , typename std::enable_if<!(std::is_same<Any&, T>::value)>::type* = nullptr // disable if value has type `any&`
        , typename std::enable_if<!(std::is_const<T>::value)>::type* = nullptr) // disable if value has type `const ValueType&&`
        : _content(new Content<typename std::decay<T>::type>(std::move(value)))
    {
    }
    Any(const Any& value)
        : _content(value._content ? value._content->Clone() : nullptr)
    {
    }
    Any(Any&& value)
        : _content(value._content)
    {
        value._content = nullptr;
    }
    ~Any() noexcept
    {
        delete _content;
    }
    Any& swap(Any& other) noexcept
    {
        ContentBase* tmp = other._content;
        other._content = _content;
        _content = tmp;
        return *this;
    }
    template<typename T>
    Any& operator=(const T& rhs)
    {
        Any(rhs).swap(*this);
        return *this;
    }
    template<typename T>
    Any& operator=(T&& rhs)
    {
        Any(std::move(rhs)).swap(*this);
        return *this;
    }
    bool IsEmpty() const noexcept
    {
        return !_content;
    }
    void Clear() noexcept
    {
        Any().swap(*this);
    }
    template<typename T>
    T* Get()
    {
        auto object = dynamic_cast<
            Content<
                typename ::remove_cv<
                    typename std::decay<T>::type
                >::type
            >
        >(_content);
        if (object == nullptr)
        {
            throw std::logic_error("Invalid type or content is empty");
        }
        return object->_value;
    }
};


#endif // !ANY_H

#pragma once
#ifndef PROPERTY_H
#define PROPERTY_H

#include "Template.h"
#include <functional>
namespace pl
{
/*
APIԼ����
getter:
������const T(const T&)�ĺ���
�ڻ�ȡ���Ե�ʱ��ᱻAPI����
���в���const T&���÷�װ���ֶε�ֵ�����û���getter�д���������

setter:
������void(T&, const T&)�ĺ���
���趨���Ե�ʱ��ᱻAPI����
����T&��Ҫ���޸ĵ������ڲ��ֶε����ã�const T&���µ�ֵ
���û���setter�д�����ֵ��Ȼ����ڲ��ֶ������޸�
*/

template<typename T>
using PropertyGetterType = std::function<const T(const T&)>;

template<typename T>
using PropertySetterType = std::function<void(T&, const T&)>;

template<class T> inline
const T DefaultGetter(const T& value)
{
    return value;
}

template<class T> inline
void DefaultSetter(T& oldValue, const T& newValue)
{
    oldValue = newValue;
}

template<class T>
class Property
{
private:
    T _value;
    PropertyGetterType<T> _getter;
    PropertySetterType<T> _setter;
public:
    Property(PropertyGetterType<T> getter = DefaultGetter<T>,
             PropertySetterType<T> setter = DefaultSetter<T>)
        : _value()
        , _getter(getter)
        , _setter(setter)
    {
    }
    Property(const T& value,
             PropertyGetterType<T> getter = DefaultGetter<T>,
             PropertySetterType<T> setter = DefaultSetter<T>)
        : _value(value)
        , _getter(getter)
        , _setter(setter)
    {
    }
    Property(T&& value,
             PropertyGetterType<T> getter = DefaultGetter<T>,
             PropertySetterType<T> setter = DefaultSetter<T>)
        : _value(value)
        , _getter(getter)
        , _setter(setter)
    {
    }
    Property(const Property<T>& property)
        : _value(property._value)
        , _getter(property._getter)
        , _setter(property._setter)
    {
    }
    Property(Property<T>&& property)
        : _value(RvalueCast(property._value))
        , _getter(property._getter)
        , _setter(property._setter)
    {
    }
    Property<T>& operator=(const T& value)
    {
        _setter(_value, value);
        return *this;
    }
    Property<T>& operator=(const Property<T>& value)
    {
        _setter(_value, value.operator const T());
        return *this;
    }
    operator const T()
    {
        return _getter(_value);
    }
};

}

#endif // !PROPERTY_H

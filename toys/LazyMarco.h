#pragma once

#define CONST_REF_GETTER(type, name, raw_name) \
    const type& name() const { return raw_name; }

#define MUTABLE_GETTER(type, name, raw_name) \
    type& name() const { return raw_name; }

#define REF_GETTER(type, name, raw_name) \
    type name() { return _raw_name; }

#define VALUE_SETTER(type, name, raw_name) \
    void name(const type __value) { raw_name = __value; }

#define REF_SETTER(type, name, raw_name) \
    void name(const type& __value) { raw_name = __value; }

#define RVALUE_SETTER(type, name, raw_name) \
    void name(type&& __value) { raw_name = std::move(__value); }


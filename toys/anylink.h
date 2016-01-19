#pragma once
#ifndef ANYLINK_H
#define ANYLINK_H

#include "any.h"

struct LinkNode
{
    Any Value;
    LinkNode* Prev;
    LinkNode* Next;
    template<typename T>
    LinkNode(const T& value, LinkNode* prev, LinkNode* next)
        : Value(value)
        , Prev(prev)
        , Next(next)
    {
    }
};

class LinkedList
{
protected:
    LinkNode* _head;
    LinkNode* _tail;
public:
    using iterator = LinkNode*;

    LinkedList()
        : _head(nullptr)
        , _tail(nullptr)
    {
    }

    ~LinkedList()
    {
        while (_head)
        {
            LinkNode* tmp = _head;
            _head = _head->Next;
            delete tmp;
        }
    }

    template<typename T>
    void AddBack(T value)
    {
        LinkNode* newNode = new LinkNode(value, _tail, nullptr);
        if (_head == nullptr)
        {
            _head = newNode;
            _tail = newNode;
            return;
        }
        _tail->Next = newNode;
        _tail = newNode;
    }
    template<typename T>
    void AddFront(T value)
    {
        LinkNode* newNode = new LinkNode(value, nullptr, _head);
        if (_head == nullptr)
        {
            _head = newNode;
            _tail = newNode;
            return;
        }
        _head->Prev = newNode;
        _head = newNode;
    }
    iterator begin()
    {
        return _head;
    }
    iterator end()
    {
        return nullptr;
    }
    template<typename T>
    T& Get(iterator index)
    {
        return &(index->Value.Get<T>());
    }
};


#endif // !ANYLINK_H

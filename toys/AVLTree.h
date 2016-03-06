#pragma once

#ifndef TREE_H
#define TREE_H

#include <allocators>
#include <functional>
#include <algorithm>
#include <utility>
namespace pl
{
namespace container
{

template<typename T>
struct _simple_types
{
    using size_type = size_t;
    using difference_type = ptrdiff_t;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;
};

template<class T>
struct _balenced_tree_node
{
    using _node_ptr = _balenced_tree_node<T>*;
    using _value_type = T;
    _node_ptr _parent;
    _node_ptr _left;
    _node_ptr _right;
    _value_type _value;
private://non-copyable
    _balenced_tree_node& operator=(const _balenced_tree_node&);
};

template<class T>
struct _tree_simple_types: public _simple_types<T>
{
    using _node = _balenced_tree_node<T>;
    using _node_ptr = _node*;
};

template<class TPair, class TAllocator>
struct _balenced_tree_base_types
{
    using _allocator      = TAllocator;
    using _self           = _balenced_tree_base_types<TPair, TAllocator>;
    using _allocator_pair = typename std::allocator_traits<_allocator>::template rebind<TPair>::other;
    using _node           = _balenced_tree_node<typename _allocator_pair::value_type>;
    using _allocator_node = typename _allocator_pair::template rebind<_node>::other;
    using _node_ptr       = _allocator_node::pointer;
    using _node_ptr_ref   = _node_ptr&;

    using _value_types = _tree_simple_types<TPair>;
};

template<class TKey,
         class TValue,
         class TCompare,
         class TAllocator,
         bool  VIsMultipleKey>
struct _balenced_tree_traits
{
    using _self          = _balenced_tree_traits<TKey, TValue, TCompare, TAllocator, VIsMultipleKey>;
    using key_type       = TKey;
    using value_type     = std::pair<const TKey, TValue>;
    using key_compare    = TKey;
    using allocator_type = TAllocator;

    const static bool _IsMultiKey = VIsMultipleKey;
    
    struct value_compare
    {
        friend class _self;
        using first_argument_type  = value_type;
        using second_argument_type = value_type;
        using result_type          = bool;

        bool operator()(const value_type& left, const value_type& right) const
        {
            return _comparer(left.first, right.first);
        }
        value_compare(key_compare comparer)
            :_comparer(comparer)
        {
        }
    protected:
        key_compare _comparer;
    };

    template<class T1, class T2>
    static const T1& _get_key(const std::pair<T1, T2>& value)
    {
        return value.first;
    }
};

template<class _value_types>
class _balenced_tree_data
{
public:
    using _self = _balenced_tree_data<_value_types>;
    using _node_ptr = _value_types::_node_ptr;
    using _node_ptr_ref = _node_ptr&;

    using value_type      = typename _value_types::value_type;
    using size_type       = typename _value_types::size_type;
    using difference_type = typename _value_types::difference_type;
    using pointer         = typename _value_types::pointer;
    using const_pointer   = typename _value_types::const_pointer;
    using reference       = typename _value_types::reference;
    using const_reference = typename _value_types::const_reference;

private:
    _node_ptr _head;
    size_type _size;
public:
    _balenced_tree_data()
        : _head(nullptr)
        , _size(0)
    {
    }
    static _node_ptr_ref _left(_node_ptr pnode)
    {
        return pnode->_left;
    }
    static _node_ptr_ref _right(_node_ptr pnode)
    {
        return pnode->_right;
    }
    static _node_ptr_ref _parent(_node_ptr pnode)
    {
        return pnode->_parent;
    }
    static reference _value(_node_ptr pnode)
    {
        return pnode->_value;
    }
    static _node_ptr _max(_node_ptr pnode)
    {
        while (_right(pnode) != nullptr)
        {
            pnode = _right(pnode);
        }
        return pnode;
    }
    static _node_ptr _min(_node_ptr pnode)
    {
        while (_left(pnode) != nullptr)
        {
            pnode = _left(pnode);
        }
        return pnode;
    }
};

template<class TBalencedTreeTraits>
class _balenced_tree_base
{
public:
    using _traits = TBalencedTreeTraits;

    using _self = _balenced_tree_base<_traits>;
    
    using allocator_type = typename _traits::allocator_type;
    using key_compare    = typename _traits::key_compare;

    using _allocator_types = _balenced_tree_base_types<typename _traits::value_type, allocator_type>;

    using _allocator      = typename _allocator_types::_allocator;
    using _allocator_node = typename _allocator_types::_allocator_node;
    using _node           = typename _allocator_types::_node;
    using _node_ptr       = typename _allocator_types::_node_ptr;
    using _value_types    = typename _allocator_types::_value_types;

    using _node_ptr_ref = _node_ptr&;

    using value_type      = typename _value_types::value_type;
    using size_type       = typename _value_types::size_type;
    using difference_type = typename _value_types::difference_type;
    using pointer         = typename _value_types::pointer;
    using const_pointer   = typename _value_types::const_pointer;
    using reference       = typename _value_types::const_pointer;
    using const_reference = typename _value_types::const_reference;
private:
    key_compare _key_compare;
    _allocator_node _node_allocator;
    _balenced_tree_data<_value_types> _data;
public:
    using _data_type = _balenced_tree_data<_value_types>;
    static _node_ptr_ref _left(_node_ptr pnode)
    {
        return _data_type::_left(pnode);
    }
    static _node_ptr_ref _right(_node_ptr pnode)
    {
        return _data_type::_right(pnode);
    }
    static _node_ptr_ref _parent(_node_ptr pnode)
    {
        return _data_type::_parent(pnode);
    }
    static reference _value(_node_ptr pnode)
    {
        return _data_type::_value(pnode);
    }
    static _node_ptr _max(_node_ptr pnode)
    {
        return _data_type::_max(pnode);
    }
    static _node_ptr _min(_node_ptr pnode)
    {
        return _data_type::_min(pnode);
    }
    _balenced_tree_base(const key_compare& comparer, const allocator_type& allocator)
        : _key_compare(comparer)
        , _node_allocator(allocator)
        , _data()
    {
        _construct();
    }
    _balenced_tree_base(const key_compare& comparer, allocator_type&& allocator)
        : _key_compare(comparer)
        , _node_allocator(std::move(allocator))
        , _data()
    {
        _construct();
    }
    
    key_compare& _get_compare() noexcept
    {
        return _key_compare;
    }
    const key_compare& _get_compare() const noexcept
    {
        return _key_compare;
    }
    _allocator_node& _get_allocator() noexcept
    {
        return _node_allocator;
    }
    const _allocator_node& _get_allocator() const noexcept
    {
        return _node_allocator;
    }
};

template<typename T,
    typename TComparer  = std::less<T>,
    typename TAllocator = std::allocator<T>>
class banlenced_tree
{
public:
    using value_type = T;
    using allocator_type = TAllocator;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = std::allocator_traits<allocator_type>::pointer;
    using const_pointer = std::allocator_traits<allocator_type>::const_pointer;
    struct value_compare
    {
    public:
        using first_argument_type = value_type;
        using second_argument_type = value_type;
        using result_type = bool;
        value_compare(TComparer comparer)
            : _comparer(comparer)
        {
        }
        result_type operator()(first_argument_type first, second_argument_type second) const
        {
            return _comparer(first, second);
        }
    protected:
        TComparer _comparer;
    };
protected:
    struct _tree_node
    {
        value_type  value;
        _tree_node* left;
        _tree_node* right;
    };

    _tree_node* _root;

    _tree_node _insert(_tree_node* node, const value_type& T)
    {
        if (TComparer(T, node->value))
        {
            if (right == nullptr)
            {
                right =
            }
        }
    }
public:
    banlenced_tree()
        : _root(nullptr)
    {
    }

    void insert(const value_type& T)
    {

    }

};

}

};



#endif // !TREE_H

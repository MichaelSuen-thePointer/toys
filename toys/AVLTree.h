#pragma once

#ifndef TREE_H
#define TREE_H

#include <allocators>
#include <functional>
#include <algorithm>
#include "extension.h"
namespace pl
{
namespace container
{

template<class TTree>
class _binary_tree_unchecked_const_iterator
{
public:
    using _self = _binary_tree_unchecked_const_iterator<TTree>;
    using _tree = TTree;
    using iterator_category = std::bidirectional_iterator_tag;
    using _node_ptr = _tree::_node_ptr;
    using value_type = _tree::value_type;
    using difference_type = _tree::difference_type;
    using pointer = _tree::const_pointer;
    using reference = _tree::reference;

public:
    _node_ptr _ptr;

    _binary_tree_unchecked_const_iterator(_node_ptr pnode = nullptr)
        : _ptr(pnode)
    {
    }
    reference operator*() const
    {
        return _tree::_value(_ptr);
    }
    pointer operator->() const
    {
        return std::pointer_traits<pointer>::pointer_to(**this);
    }
    
    //pre-increment
    _self& operator++()
    {
        if (!_tree::_is_valid(_ptr))
        {

        }
        else if (_tree::_is_valid(_tree::_right(_ptr)))
        {
            _ptr = _tree::_min(_tree::_right(_ptr));
        }
        else
        {
            _node_ptr pnode;
            while (_tree::_is_valid(_pnode = _tree::_parent(_ptr)) &&
                   _ptr == _tree::_right(_pnode))
            {
                _ptr = _pnode;
            }
            _ptr = _pnode;  

        }
        return *this;
    }

    _self operator++(int)
    {
        _self tmp = *this;
        ++*this;
        return tmp;
    }

    _self& operator--()
    {
        if (!_tree::_is_valid(_ptr))
        {
        }
        else if (_tree::_is_valid(_tree::_left(_ptr)))
        {
            _ptr = _tree::_max(_tree::_left(_ptr));
        }
        else
        {
            _node_ptr pnode;
            while (_tree::_is_valid(pnode = _tree::_parent(_ptr)) &&
                   _ptr == _tree::_left(_pnode))
            {
                _ptr = _pnode;
            }
            if (_tree::_is_valid(_ptr))
            {
                _ptr = pnode;
            }
        }
        return *this;
    }
};

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
    bool operator==(_node_ptr pnode)
    {
        return pnode == _head;
    }
    static bool _is_null(_node_ptr pnode)
    {
        return pnode == _node_ptr();
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
    static bool _is_head(_node_ptr pnode)
    {
        return _data == pnode;
    }
    static bool _is_null(_node_ptr pnode)
    {
        return _data_type::_is_null(pnode);
    }
    static bool _is_valid(_node_ptr pnode)
    {
        return !_is_head(pnode) && !_is_null(pnode);
    }
    static _node_ptr _max(_node_ptr pnode)
    {
        return _data_type::_max(pnode);
    }
    static _node_ptr _min(_node_ptr pnode)
    {
        return _data_type::_min(pnode);
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
    _data_type& _get_data() noexcept
    {
        return _data;
    }
    const _data_type& _get_data() const noexcept
    {
        return _data;
    }
    _node_ptr& _head() noexcept
    {
        return _get_data()._head;
    }
    const _node_ptr& _head() const noexcept
    {
        return _get_data()._head;
    }
    size_type& _size() noexcept
    {
        return _get_data()._size;
    }
    const size_type& _size() const noexcept
    {
        return _get_data()._size;
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
    
    ~_balenced_tree_base() noexcept
    {
        _free_head_node(_head());
    }

    void _construct()
    {
        _head() = _buy_head_node();
    }

    void _copy_allocator(const _allocator_node& allocator)
    {
        _pocca(_get_allocator(), allocator);
    }

    void _move_allocator(const _allocator_node& allocator)
    {
        _pocma(_get_allocator(), allocator);
    }

    void _swap_allocator(const _allocator_node& allocator)
    {
        _pocs(_get_allocator(), allocator);
    }

    _node_ptr _buy_head_node()
    {
        _node_ptr pnode = _get_allocator().allocate(1);

        try
        {
            _get_allocator().construct(std::addressof(_left(pnode)), pnode);
            _get_allocator().construct(std::addressof(_right(pnode)), pnode);
            _get_allocator().construct(std::addressof(_parent(pnode)), pnode);
        }
        catch (...)
        {
            _get_allocator().deallocate(pnode, 1);
            throw;
        }

        return pnode;
    }

    void _free_head_node(_node_ptr pnode)
    {
        _get_allocator().destroy(std::addressof(_left(pnode)));
        _get_allocator().destroy(std::addressof(_right(pnode)));
        _get_allocator().destroy(std::addressof(_parent(pnode)));
        _get_allocator().deallocate(pnode, 1);
    }

    void _buy_nonvalue_node()
    {
        _node_ptr pnode = _get_allocator().allocate(1);
        try
        {
            _get_allocator().construct(std::addressof(_left(pnode)), _head());
            _get_allocator().construct(std::addressof(_right(pnode)), _head());
            _get_allocator().construct(std::addressof(_parent(pnode)), _head());
        }
        catch (...)
        {
            _get_allocator().deallocate(pnode, 1);
            throw;
        }
        return pnode;
    }

    void _free_nonvalue_node(_node_ptr pnode)
    {
        _free_head_node(pnode);
    }

    template<class... TArgs>
    _node_ptr _buy_node(TArgs&&... args)
    {
        _node_ptr pnode = _buy_nonvalue_node();
        try
        {
            _get_allocator().construct(
                std::addressof(_value(pnode)),
                std::forward<TArgs>(args)...);
        }
        catch (...)
        {
            _free_nonvalue_node(pnode);
            throw;
        }
        return pnode;
    }
};

template<class TBalencedTreeTraits>
class binary_tree: public _balenced_tree_base<TBalencedTreeTraits>
{
public:
    using _traits = TBalencedTreeTraits;
    using _self = binary_tree<_traits>;
    using _base = _balenced_tree_base<_traits>;

    using key_type = _traits::key_type;
    using value_compare = _traits::value_compare;

    const static bool _IsMultiKey = _traits::_IsMultiKey;

    using _node           = typename _base::_node;
    using _node_ptr       = typename _base::_node_ptr;
    using _allocator_node = typename _base::_allocator_node;
    
    using key_compare    = typename _base::key_compare;
    using allocator_type = typename _base::allocator_type;
    
    using value_type      = typename _base::value_type;
    using size_type       = typename _base::size_type;
    using difference_type = typename _base::difference_type;
    using pointer         = typename _base::pointer;
    using const_pointer   = typename _base::const_pointer;
    using reference       = typename _base::reference;
    using const_reference = typename _base::const_reference;
    
    struct _copy_tag {};
    struct _move_tag {};

    size_type size() const noexcept
    {
        return _base::_size();
    }

    size_type max_size() const noexcept
    {
        return _base::_get_allocator().max_size();
    }

    bool empty() const noexcept
    {
        return size() == 0;
    }

    allocator_type get_allocator() const noexcept
    {
        return _base::_get_allocator();
    }

    key_compare key_comp() const
    {
        return _base::_get_compare();
    }

    value_compare value_comp() const
    {
        return value_compare(key_comp());
    }

    binary_tree(const key_compare& compare, const allocator_type& allocator)
        : _base(compare, allocator)
    {
    }

    binary_tree(const _self& right, const allocator_type& allocator)
        : _base(right.key_comp(), right.get_allocator())
    {
        try
        {
            _copy(right, _copy_tag());
        }
        catch (...)
        {
            _tidy();
            throw;
        }
    }

    binary_tree(_self&& right)
        : _base(right.key_comp(), std::move(right._base::_get_allocator()))
    {
        _assign_rv(std::forward<_self>(right), std::true_type());
    }

    binary_tree(_self&& right, const allocator_type& allocator)
        : _base(right.key_comp(), allocator)
    {
        _assign_rv(std::forward<_self>(right));
    }

    _self& operator=(_self&& right)
    {
        if (this != &right)
        {
            clear();
            if (_allocator_node::propagate_on_container_move_assignment::value &&
                _base::_get_allocator() != right._base::_get_allocator())
            {
                _base::_move_allocator(right._base::_get_allocator());
            }
            _assign_rv(std::forward<_self>(right));
        }
        return *this;
    }

    void _assign_rv(_self&& right, std::true_type)
    {
        std::swap(_base::_get_compare(), _right._base::_get_compare());
        std::swap(_base::_head(), _right._base::_head());
        std::swap(_base::_size(), _right._base::_size());
    }

    void _assign_rv(_self&& right, std::false_type)
    {
        if (get_allocator() == right.get_allocator())
        {
            _assign_rv(std::forward<_self>(_right), std::true_type());
        }
        else
        {
            _copy(_right);
        }
    }

    void _assign_rv(_self&& right)
    {
        _assign_rv(std::forward<_self>(right), typename _allocator_node::propagate_on_container_move_assignment());
    }



};

template<typename T,
    typename TComparer  = std::less<T>,
    typename TAllocator = std::allocator<T>>
class banlenced_tree
{
public:
    using value_type      = T;
    using allocator_type  = TAllocator;
    using reference       = value_type&;
    using const_reference = const value_type&;
    using pointer         = typename std::allocator_traits<allocator_type>::pointer;
    using const_pointer   = typename std::allocator_traits<allocator_type>::const_pointer;
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

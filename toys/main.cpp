#include <map>

using namespace std;

template<class _Value_type, class _Voidptr>
struct _Tree_node
{	// tree node
    _Voidptr        _Left;	// left subtree, or smallest element if head
    _Voidptr        _Parent;	// parent, or root of tree if head
    _Voidptr        _Right;	// right subtree, or largest element if head
    char            _Color;	// _Red or _Black, _Black if head
    char            _Isnil;	// true only if head (also nil) node
    _Value_type     _Myval;	// the stored value, unused if head

private:
    _Tree_node& operator=(const _Tree_node&);
};

template<class _Value_type>
struct _Tree_node<_Value_type, void *>
{	// tree node
    typedef _Tree_node<_Value_type, void *> *_Nodeptr;
    _Nodeptr    _Left;  	// left subtree, or smallest element if head
    _Nodeptr    _Parent;	// parent, or root of tree if head
    _Nodeptr    _Right;	    // right subtree, or largest element if head
    char        _Color;	    // _Red or _Black, _Black if head
    char        _Isnil;	    // true only if head (also nil) node
    _Value_type _Myval;	    // the stored value, unused if head

private:
    _Tree_node& operator=(const _Tree_node&);
};

template<class _Ty>
struct _Tree_simple_types: public _Simple_types<_Ty>
{	// wraps types needed by iterators
    typedef _Tree_node<_Ty, void *>     _Node;
    typedef _Node *                     _Nodeptr;
};

template<class _Ty, class _Alloc0>
struct _Tree_base_types
{	// types needed for a container base
    typedef _Alloc0                                         _Alloc;
    typedef _Tree_base_types<_Ty, _Alloc>                   _Myt;

    typedef _Wrap_alloc<_Alloc>                             _Alty0;
    typedef typename _Alty0::template rebind<_Ty>::other    _Alty;


    typedef typename _Get_voidptr<_Alty, typename _Alty::pointer>::type _Voidptr;
    typedef _Tree_node<typename _Alty::value_type, _Voidptr>            _Node;

    typedef typename _Alty::template rebind<_Node>::other   _Alnod_type;
    typedef typename _Alnod_type::pointer                   _Nodeptr;
    typedef _Nodeptr&                                       _Nodepref;

    typedef typename _If<_Is_simple_alloc<_Alty>::value,
        _Tree_simple_types<typename _Alty::value_type>,
        _Tree_iter_types<typename _Alty::value_type,
        typename _Alty::size_type,
        typename _Alty::difference_type,
        typename _Alty::pointer,
        typename _Alty::const_pointer,
        typename _Alty::reference,
        typename _Alty::const_reference,
        _Nodeptr> >::type
                                                            _Val_types;
};

// TEMPLATE CLASS _Tree_val
template<class _Val_types>
class _Tree_val
    : public _Container_base
{	// base class for tree to hold data
public:
    typedef _Tree_val<_Val_types>                   _Myt;
                                                    
    typedef typename _Val_types::_Nodeptr           _Nodeptr;
    typedef _Nodeptr&                               _Nodepref;

    typedef typename _Val_types::value_type         value_type;
    typedef typename _Val_types::size_type          size_type;
    typedef typename _Val_types::difference_type    difference_type;
    typedef typename _Val_types::pointer            pointer;
    typedef typename _Val_types::const_pointer      const_pointer;
    typedef typename _Val_types::reference          reference;
    typedef typename _Val_types::const_reference    const_reference;

    typedef _Tree_const_iterator<_Myt>              const_iterator;
    typedef _Tree_iterator<_Myt>                    iterator;

    _Tree_val()
    {	// initialize data
        this->_Myhead = 0;
        this->_Mysize = 0;
    }

    enum _Redbl
    {	// colors for link to parent
        _Red, _Black
    };

    static char& _Color(_Nodeptr _Pnode)
    {	// return reference to color in node
        return ((char&)_Pnode->_Color);
    }

    static char& _Isnil(_Nodeptr _Pnode)
    {	// return reference to nil flag in node
        return ((char&)_Pnode->_Isnil);
    }

    static _Nodepref _Left(_Nodeptr _Pnode)
    {	// return reference to left pointer in node
        return ((_Nodepref)_Pnode->_Left);
    }

    static _Nodepref _Parent(_Nodeptr _Pnode)
    {	// return reference to parent pointer in node
        return ((_Nodepref)_Pnode->_Parent);
    }

    static _Nodepref _Right(_Nodeptr _Pnode)
    {	// return reference to right pointer in node
        return ((_Nodepref)_Pnode->_Right);
    }

    static reference _Myval(_Nodeptr _Pnode)
    {	// return reference to value in node
        return ((reference)_Pnode->_Myval);
    }

    static _Nodeptr _Max(_Nodeptr _Pnode)
    {	// return rightmost node in subtree at _Pnode
        while (!_Isnil(_Right(_Pnode)))
            _Pnode = _Right(_Pnode);
        return (_Pnode);
    }

    static _Nodeptr _Min(_Nodeptr _Pnode)
    {	// return leftmost node in subtree at _Pnode
        while (!_Isnil(_Left(_Pnode)))
            _Pnode = _Left(_Pnode);
        return (_Pnode);
    }

    _Nodeptr _Myhead;	// pointer to head node
    size_type _Mysize;	// number of elements
};

template<class _Kty,	// key type
         class _Ty,	// mapped type
         class _Pr,	// comparator predicate type
         class _Alloc,	// actual allocator type (should be value allocator)
         bool  _Mfl>	// true if multiple equivalent keys are permitted
class _Tmap_traits
{	// traits required to make _Tree behave like a map
public:
    typedef _Kty                                key_type;
    typedef pair<const _Kty, _Ty>               value_type;
    typedef _Pr                                 key_compare;
    typedef _Alloc                              allocator_type;

    enum
    {	// make multi parameter visible as an enum constant
        _Multi = _Mfl
    };

    class value_compare
    {	// functor for comparing two element values
        friend class _Tmap_traits<_Kty, _Ty, _Pr, _Alloc, _Mfl>;

    public:
        typedef value_type first_argument_type;
        typedef value_type second_argument_type;
        typedef bool result_type;

        bool operator()(const value_type& _Left,
                        const value_type& _Right) const
        {	// test if _Left precedes _Right by comparing just keys
            return (comp(_Left.first, _Right.first));
        }

        value_compare(key_compare _Pred)
            : comp(_Pred)
        {	// construct with specified predicate
        }

    protected:
        key_compare comp;	// the comparator predicate for keys
    };

    template<class _Ty1,
    class _Ty2>
        static const _Kty& _Kfn(const pair<_Ty1, _Ty2>& _Val)
    {	// extract key from element value
        return (_Val.first);
    }
};

// TEMPLATE CLASS _Tree_comp_alloc
template<class _Traits>
class _Tree_comp_alloc
{	// base class for tree to hold ordering predicate, allocator
public:
    typedef _Tree_comp_alloc<_Traits>                   _Myt;

    typedef typename _Traits::allocator_type            allocator_type;
    typedef typename _Traits::key_compare               key_compare;

    typedef _Tree_base_types<typename _Traits::value_type, allocator_type>
                                                        _Alloc_types;

    typedef typename _Alloc_types::_Alloc      _Alloc;
    typedef typename _Alloc_types::_Alnod_type _Alty;
    typedef typename _Alloc_types::_Node       _Node;
    typedef typename _Alloc_types::_Nodeptr    _Nodeptr;
    typedef typename _Alloc_types::_Val_types  _Val_types;

    typedef _Nodeptr& _Nodepref;

    typedef typename _Val_types::value_type      value_type;
    typedef typename _Val_types::size_type       size_type;
    typedef typename _Val_types::difference_type difference_type;
    typedef typename _Val_types::pointer         pointer;
    typedef typename _Val_types::const_pointer   const_pointer;
    typedef typename _Val_types::reference       reference;
    typedef typename _Val_types::const_reference const_reference;

    typedef _Tree_const_iterator<_Tree_val<_Val_types> >    const_iterator;
    typedef _Tree_iterator<_Tree_val<_Val_types> >          iterator;

    enum _Redbl
    {	// colors for link to parent
        _Red, _Black
    };
private:
    _Compressed_pair<key_compare, _Compressed_pair<_Alty, _Tree_val<_Val_types> > > _Mypair;
public:
    static char& _Color(_Nodeptr _Pnode)
    {	// return reference to color in node
        return (_Tree_val<_Val_types>::_Color(_Pnode));
    }

    static char& _Isnil(_Nodeptr _Pnode)
    {	// return reference to nil flag in node
        return (_Tree_val<_Val_types>::_Isnil(_Pnode));
    }

    static _Nodepref _Left(_Nodeptr _Pnode)
    {	// return reference to left pointer in node
        return (_Tree_val<_Val_types>::_Left(_Pnode));
    }

    static _Nodepref _Parent(_Nodeptr _Pnode)
    {	// return reference to parent pointer in node
        return (_Tree_val<_Val_types>::_Parent(_Pnode));
    }

    static _Nodepref _Right(_Nodeptr _Pnode)
    {	// return reference to right pointer in node
        return (_Tree_val<_Val_types>::_Right(_Pnode));
    }

    static reference _Myval(_Nodeptr _Pnode)
    {	// return reference to value in node
        return (_Tree_val<_Val_types>::_Myval(_Pnode));
    }

    static _Nodeptr _Max(_Nodeptr _Pnode)
    {	// return rightmost node in subtree at _Pnode
        return (_Tree_val<_Val_types>::_Max(_Pnode));
    }

    static _Nodeptr _Min(_Nodeptr _Pnode)
    {	// return leftmost node in subtree at _Pnode
        return (_Tree_val<_Val_types>::_Min(_Pnode));
    }

    _Tree_comp_alloc(const key_compare& _Parg, const allocator_type& _Al)
        : _Mypair(_One_then_variadic_args_t(), _Parg,
                  _One_then_variadic_args_t(), _Al)
    {	// construct from comparator, allocator
        _Construct();
    }

    _Tree_comp_alloc(const key_compare& _Parg, allocator_type&& _Al)
        : _Mypair(_One_then_variadic_args_t(), _Parg,
                  _One_then_variadic_args_t(), _STD move(_Al))
    {	// construct from comparator, allocator
        _Construct();
    }

#if _ITERATOR_DEBUG_LEVEL == 0
    void _Construct()
    {	// construct head node
        _Myhead() = _Buyheadnode();
    }

    ~_Tree_comp_alloc() _NOEXCEPT
    {	// destroy head node
        _Freeheadnode(_Myhead());
    }

    void _Copy_alloc(const _Alty& _Al)
    {	// replace old allocator
        _Pocca(_Getal(), _Al);
    }

    void _Move_alloc(_Alty& _Al)
    {	// replace old allocator
        _Pocma(_Getal(), _Al);
    }

    void _Swap_alloc(_Myt& _Right)
    {	// swap allocators
        _Pocs(_Getal(), _Right._Getal());
    }

#else /* _ITERATOR_DEBUG_LEVEL == 0 */
    void _Construct()
    {	// construct head node, proxy
        _Myhead() = _Buyheadnode();
        _TRY_BEGIN
            _Alloc_proxy();
        _CATCH_ALL
            _Freeheadnode(_Myhead());
        _RERAISE;
        _CATCH_END
    }

    ~_Tree_comp_alloc() _NOEXCEPT
    {	// destroy head node, proxy
        _Freeheadnode(_Myhead());
        _Free_proxy();
    }

    void _Copy_alloc(const _Alty& _Al)
    {	// replace old allocator
        _Free_proxy();
        _Pocca(_Getal(), _Al);
        _Alloc_proxy();
    }

    void _Move_alloc(_Alty& _Al)
    {	// replace old allocator
        _Free_proxy();
        _Pocma(_Getal(), _Al);
        _Alloc_proxy();
    }

    void _Swap_alloc(_Myt& _Right)
    {	// swap allocators
        _Pocs(_Getal(), _Right._Getal());
        _Swap_adl(_Myproxy(), _Right._Myproxy());
    }

    void _Alloc_proxy()
    {	// construct proxy
        typename _Alty::template rebind<_Container_proxy>::other
            _Alproxy(_Getal());
        _Myproxy() = _Alproxy.allocate(1);
        _Alproxy.construct(_Myproxy(), _Container_proxy());
        _Myproxy()->_Mycont = &_Get_data();
    }

    void _Free_proxy()
    {	// destroy proxy
        typename _Alty::template rebind<_Container_proxy>::other
            _Alproxy(_Getal());
        _Orphan_all();
        _Alproxy.destroy(_Myproxy());
        _Alproxy.deallocate(_Myproxy(), 1);
        _Myproxy() = 0;
    }

    _Iterator_base12 **_Getpfirst() const
    {	// get address of iterator chain
        return (_Get_data()._Getpfirst());
    }

    _Container_proxy * & _Myproxy() _NOEXCEPT
    {	// return reference to _Myproxy
        return (_Get_data()._Myproxy);
    }

    _Container_proxy * const & _Myproxy() const _NOEXCEPT
    {	// return const reference to _Myproxy
        return (_Get_data()._Myproxy);
    }
#endif /* _ITERATOR_DEBUG_LEVEL == 0 */

    void _Orphan_all()
    {	// orphan all iterators
        _Get_data()._Orphan_all();
    }

    void _Swap_all(_Myt& _Right)
    {	// swap all iterators
        _Get_data()._Swap_all(_Right._Get_data());
    }

    _Nodeptr _Buyheadnode()
    {	// get head node using current allocator
        _Nodeptr _Pnode = _Getal().allocate(1);

        _TRY_BEGIN
            _Getal().construct(
                _STD addressof(_Left(_Pnode)), _Pnode);
        _Getal().construct(
            _STD addressof(_Parent(_Pnode)), _Pnode);
        _Getal().construct(
            _STD addressof(_Right(_Pnode)), _Pnode);
        _CATCH_ALL
            _Getal().deallocate(_Pnode, 1);
        _RERAISE;
        _CATCH_END

            _Color(_Pnode) = _Black;
        _Isnil(_Pnode) = true;
        return (_Pnode);
    }

    void _Freeheadnode(_Nodeptr _Pnode)
    {	// free head node using current allocator
        _Getal().destroy(
            _STD addressof(_Left(_Pnode)));
        _Getal().destroy(
            _STD addressof(_Parent(_Pnode)));
        _Getal().destroy(
            _STD addressof(_Right(_Pnode)));
        _Getal().deallocate(_Pnode, 1);
    }

    _Nodeptr _Buynode0()
    {	// allocate a non-value node
        _Nodeptr _Pnode = _Getal().allocate(1);

        _TRY_BEGIN
            _Getal().construct(
                _STD addressof(_Left(_Pnode)), _Myhead());
        _Getal().construct(
            _STD addressof(_Parent(_Pnode)), _Myhead());
        _Getal().construct(
            _STD addressof(_Right(_Pnode)), _Myhead());
        _CATCH_ALL
            _Getal().deallocate(_Pnode, 1);
        _RERAISE;
        _CATCH_END

            return (_Pnode);
    }

    void _Freenode0(_Nodeptr _Pnode)
    {	// free non-value node using current allocator
        _Getal().destroy(
            _STD addressof(_Left(_Pnode)));
        _Getal().destroy(
            _STD addressof(_Parent(_Pnode)));
        _Getal().destroy(
            _STD addressof(_Right(_Pnode)));
        _Getal().deallocate(_Pnode, 1);
    }

    template<class... _Valty>
    _Nodeptr _Buynode(_Valty&&... _Val)
    {	// allocate a node with defaults and set links and value
        _Nodeptr _Pnode = _Buynode0();

        this->_Color(_Pnode) = _Red;
        this->_Isnil(_Pnode) = false;

        _TRY_BEGIN
            this->_Getal().construct(
                _STD addressof(_Myval(_Pnode)),
                _STD forward<_Valty>(_Val)...);
        _CATCH_ALL
            _Freenode0(_Pnode);
        _RERAISE;
        _CATCH_END

            return (_Pnode);
    }


    key_compare& _Getcomp() _NOEXCEPT
    {	// return reference to ordering predicate
        return (_Mypair._Get_first());
    }

    const key_compare& _Getcomp() const _NOEXCEPT
    {	// return const reference to ordering predicate
        return (_Mypair._Get_first());
    }

    _Alty& _Getal() _NOEXCEPT
    {	// return reference to allocator
        return (_Mypair._Get_second()._Get_first());
    }

    const _Alty& _Getal() const _NOEXCEPT
    {	// return const reference to allocator
        return (_Mypair._Get_second()._Get_first());
    }

    _Tree_val<_Val_types>& _Get_data() _NOEXCEPT
    {	// return reference to _Tree_val
        return (_Mypair._Get_second()._Get_second());
    }

    const _Tree_val<_Val_types>& _Get_data() const _NOEXCEPT
    {	// return const reference to _Tree_val
        return (_Mypair._Get_second()._Get_second());
    }

    _Nodeptr& _Myhead() _NOEXCEPT
    {	// return reference to _Myhead
        return (_Get_data()._Myhead);
    }

    const _Nodeptr& _Myhead() const _NOEXCEPT
    {	// return const reference to _Myhead
        return (_Get_data()._Myhead);
    }

    size_type& _Mysize() _NOEXCEPT
    {	// return reference to _Mysize
        return (_Get_data()._Mysize);
    }

    const size_type& _Mysize() const _NOEXCEPT
    {	// return const reference to _Mysize
        return (_Get_data()._Mysize);
    }


};

// TEMPLATE CLASS _Tree
template<class _Traits>
class _Tree
    : public _Tree_comp_alloc<_Traits>
{	// ordered red-black tree for map/multimap/set/multiset
public:
    typedef _Tree<_Traits> _Myt;
    typedef _Tree_comp_alloc<_Traits> _Mybase;

    typedef typename _Traits::key_type key_type;
    typedef typename _Traits::value_compare value_compare;
    enum
    {	// get multi parameter
        _Multi = _Traits::_Multi
    };

    typedef typename _Mybase::_Node _Node;
    typedef typename _Mybase::_Nodeptr _Nodeptr;
    typedef typename _Mybase::_Alty _Alty;

    typedef typename _Mybase::key_compare key_compare;
    typedef typename _Mybase::allocator_type allocator_type;

    typedef typename _Mybase::value_type value_type;
    typedef typename _Mybase::size_type size_type;
    typedef typename _Mybase::difference_type difference_type;
    typedef typename _Mybase::pointer pointer;
    typedef typename _Mybase::const_pointer const_pointer;
    typedef typename _Mybase::reference reference;
    typedef typename _Mybase::const_reference const_reference;

    typedef typename _Mybase::const_iterator const_iterator;
    typedef typename _If<is_same<key_type, value_type>::value,
        typename _Mybase::const_iterator,
        typename _Mybase::iterator>::type iterator;

    typedef _STD reverse_iterator<iterator> reverse_iterator;
    typedef _STD reverse_iterator<const_iterator> const_reverse_iterator;

    typedef pair<iterator, bool> _Pairib;
    typedef pair<iterator, iterator> _Pairii;
    typedef pair<const_iterator, const_iterator> _Paircc;

    struct _Copy_tag
    {	// signals copying
    };
    struct _Move_tag
    {	// signals moving
    };

    _Tree(const key_compare& _Parg,
          const allocator_type& _Al)
        : _Mybase(_Parg, _Al)
    {	// construct empty tree
    }

    _Tree(const _Myt& _Right, const allocator_type& _Al)
        : _Mybase(_Right.key_comp(), _Al)
    {	// construct tree by copying _Right, allocator
        _TRY_BEGIN
            _Copy(_Right, _Copy_tag());
        _CATCH_ALL
            _Tidy();
        _RERAISE;
        _CATCH_END
    }

    _Tree(_Myt&& _Right)
        : _Mybase(_Right.key_comp(), _STD move(_Right._Getal()))
    {	// construct tree by moving _Right
        _Assign_rv(_STD forward<_Myt>(_Right), true_type());
    }

    _Tree(_Myt&& _Right, const allocator_type& _Al)
        : _Mybase(_Right.key_comp(), _Al)
    {	// construct tree by moving _Right, allocator
        _Assign_rv(_STD forward<_Myt>(_Right));
    }

    _Myt& operator=(_Myt&& _Right)
    {	// assign by moving _Right
        if (this != &_Right)
        {	// different, move it
            clear();
            if (_Alty::propagate_on_container_move_assignment::value
                && this->_Getal() != _Right._Getal())
                this->_Move_alloc(_Right._Getal());

            _Assign_rv(_STD forward<_Myt>(_Right));
        }
        return (*this);
    }

    void _Assign_rv(_Myt&& _Right, true_type)
    {	// move from _Right, stealing its contents
        this->_Swap_all(_Right);
        _Swap_adl(this->_Getcomp(), _Right._Getcomp());
        _Swap_adl(this->_Myhead(), _Right._Myhead());
        _STD swap(this->_Mysize(), _Right._Mysize());
    }

    void _Assign_rv(_Myt&& _Right, false_type)
    {	// move from _Right, possibly moving its contents
        if (get_allocator() == _Right.get_allocator())
            _Assign_rv(_STD forward<_Myt>(_Right), true_type());
        else
            _Copy(_Right, _Move_tag());
    }

    void _Assign_rv(_Myt&& _Right)
    {	// assign by moving _Right
        _Assign_rv(_STD forward<_Myt>(_Right),
                   typename _Alty::propagate_on_container_move_assignment());
    }


    _Pairib insert(value_type&& _Val)
    {	// try to insert node with value _Val, favoring right side
        return (_Insert_nohint(false,
                               _STD forward<value_type>(_Val), _Nil()));
    }

    iterator insert(const_iterator _Where, value_type&& _Val)
    {	// try to insert node with value _Val using _Where as a hint
        return (_Insert_hint(_Where,
                             _STD forward<value_type>(_Val), _Nil()));
    }

    template<class _Valty>
    typename enable_if<is_convertible<_Valty, value_type>::value,
        _Pairib>::type
        insert(_Valty&& _Val)
    {	// try to insert node with value _Val, favoring right side
        _Nodeptr _Newnode = this->_Buynode(_STD forward<_Valty>(_Val));
        return (_Insert_nohint(false,
                               this->_Myval(_Newnode), _Newnode));
    }

    template<class _Valty>
    typename enable_if<is_convertible<_Valty, value_type>::value,
        iterator>::type
        insert(const_iterator _Where, _Valty&& _Val)
    {	// try to insert node with value _Val using _Where as a hint
        _Nodeptr _Newnode = this->_Buynode(_STD forward<_Valty>(_Val));
        return (_Insert_hint(_Where,
                             this->_Myval(_Newnode), _Newnode));
    }

    template<class... _Valty>
    _Pairib emplace(_Valty&&... _Val)
    {	// try to insert value_type(_Val...), favoring right side
        _Nodeptr _Newnode = this->_Buynode(_STD forward<_Valty>(_Val)...);
        return (_Insert_nohint(false,
                               this->_Myval(_Newnode), _Newnode));
    }

    template<class... _Valty>
    iterator emplace_hint(const_iterator _Where, _Valty&&... _Val)
    {	// insert value_type(_Val...) at _Where
        _Nodeptr _Newnode = this->_Buynode(_STD forward<_Valty>(_Val)...);
        return (_Insert_hint(_Where,
                             this->_Myval(_Newnode), _Newnode));
    }


    ~_Tree() _NOEXCEPT
    {	// destroy tree
        _Tidy();
    }

    _Myt& operator=(const _Myt& _Right)
    {	// replace contents from _Right
        if (this != &_Right)
        {	// different, assign it
            clear();
            if (this->_Getal() != _Right._Getal()
                && _Alty::propagate_on_container_copy_assignment::value)
                this->_Copy_alloc(_Right._Getal());

            this->_Getcomp() = _Right._Getcomp();
            _Copy(_Right, _Copy_tag());
        }
        return (*this);
    }

    iterator begin() _NOEXCEPT
    {	// return iterator for beginning of mutable sequence
        return (iterator(_Lmost(), &this->_Get_data()));
    }

    const_iterator begin() const _NOEXCEPT
    {	// return iterator for beginning of nonmutable sequence
        return (const_iterator(_Lmost(), &this->_Get_data()));
    }

    iterator end() _NOEXCEPT
    {	// return iterator for end of mutable sequence
        return (iterator(this->_Myhead(), &this->_Get_data()));
    }

    const_iterator end() const _NOEXCEPT
    {	// return iterator for end of nonmutable sequence
        return (const_iterator(this->_Myhead(), &this->_Get_data()));
    }

    reverse_iterator rbegin() _NOEXCEPT
    {	// return iterator for beginning of reversed mutable sequence
        return (reverse_iterator(end()));
    }

    const_reverse_iterator rbegin() const _NOEXCEPT
    {	// return iterator for beginning of reversed nonmutable sequence
        return (const_reverse_iterator(end()));
    }

    reverse_iterator rend() _NOEXCEPT
    {	// return iterator for end of reversed mutable sequence
        return (reverse_iterator(begin()));
    }

    const_reverse_iterator rend() const _NOEXCEPT
    {	// return iterator for end of reversed nonmutable sequence
        return (const_reverse_iterator(begin()));
    }

    const_iterator cbegin() const _NOEXCEPT
    {	// return iterator for beginning of nonmutable sequence
        return (begin());
    }

    const_iterator cend() const _NOEXCEPT
    {	// return iterator for end of nonmutable sequence
        return (end());
    }

    const_reverse_iterator crbegin() const _NOEXCEPT
    {	// return iterator for beginning of reversed nonmutable sequence
        return (rbegin());
    }

    const_reverse_iterator crend() const _NOEXCEPT
    {	// return iterator for end of reversed nonmutable sequence
        return (rend());
    }

    size_type size() const _NOEXCEPT
    {	// return length of sequence
        return (this->_Mysize());
    }

    size_type max_size() const _NOEXCEPT
    {	// return maximum possible length of sequence
        return (this->_Getal().max_size());
    }

    bool empty() const _NOEXCEPT
    {	// return true only if sequence is empty
        return (size() == 0);
    }

    allocator_type get_allocator() const _NOEXCEPT
    {	// return allocator object for values
        return (this->_Getal());
    }

    key_compare key_comp() const
    {	// return object for comparing keys
        return (this->_Getcomp());
    }

    value_compare value_comp() const
    {	// return object for comparing values
        return (value_compare(key_comp()));
    }

    _Pairib insert(const value_type& _Val)
    {	// try to insert node with value _Val, favoring right side
        return (_Insert_nohint(false,
                               _Val, _Nil()));
    }

    iterator insert(const_iterator _Where,
                    const value_type& _Val)
    {	// try to insert node with value _Val using _Where as a hint
        return (_Insert_hint(_Where,
                             _Val, _Nil()));
    }

    template<class _Iter>
    void insert(_Iter _First, _Iter _Last)
    {	// insert [_First, _Last) one at a time
        _DEBUG_RANGE(_First, _Last);
        for (; _First != _Last; ++_First)

            emplace_hint(end(), *_First);
    }

    void insert(_XSTD initializer_list<value_type> _Ilist)
    {	// insert initializer_list
        insert(_Ilist.begin(), _Ilist.end());
    }

    iterator erase(const_iterator _Where)
    {	// erase element at _Where
#if _ITERATOR_DEBUG_LEVEL == 2
        if (_Where._Getcont() != &this->_Get_data()
            || this->_Isnil(_Where._Mynode()))
            _DEBUG_ERROR("map/set erase iterator outside range");
        _Nodeptr _Erasednode = _Where._Mynode();	// node to erase
        ++_Where;	// save successor iterator for return
        _Orphan_ptr(_Erasednode);

#else /* _ITERATOR_DEBUG_LEVEL == 2 */
        _Nodeptr _Erasednode = _Where._Mynode();	// node to erase
        ++_Where;	// save successor iterator for return
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

        _Nodeptr _Fixnode;	// the node to recolor as needed
        _Nodeptr _Fixnodeparent;	// parent of _Fixnode (which may be nil)
        _Nodeptr _Pnode = _Erasednode;

        if (this->_Isnil(this->_Left(_Pnode)))
            _Fixnode = this->_Right(_Pnode);	// stitch up right subtree
        else if (this->_Isnil(this->_Right(_Pnode)))
            _Fixnode = this->_Left(_Pnode);	// stitch up left subtree
        else
        {	// two subtrees, must lift successor node to replace erased
            _Pnode = _Where._Mynode();	// _Pnode is successor node
            _Fixnode = this->_Right(_Pnode);	// _Fixnode is only subtree
        }

        if (_Pnode == _Erasednode)
        {	// at most one subtree, relink it
            _Fixnodeparent = this->_Parent(_Erasednode);
            if (!this->_Isnil(_Fixnode))
                this->_Parent(_Fixnode) = _Fixnodeparent;	// link up

            if (_Root() == _Erasednode)
                _Root() = _Fixnode;	// link down from root
            else if (this->_Left(_Fixnodeparent) == _Erasednode)
                this->_Left(_Fixnodeparent) = _Fixnode;	// link down to left
            else
                this->_Right(_Fixnodeparent) =
                _Fixnode;	// link down to right

            if (_Lmost() == _Erasednode)
                _Lmost() = this->_Isnil(_Fixnode)
                ? _Fixnodeparent	// smallest is parent of erased node
                : this->_Min(_Fixnode);	// smallest in relinked subtree

            if (_Rmost() == _Erasednode)
                _Rmost() = this->_Isnil(_Fixnode)
                ? _Fixnodeparent	// largest is parent of erased node
                : this->_Max(_Fixnode);	// largest in relinked subtree
        }
        else
        {	// erased has two subtrees, _Pnode is successor to erased
            this->_Parent(this->_Left(_Erasednode)) =
                _Pnode;	// link left up
            this->_Left(_Pnode) =
                this->_Left(_Erasednode);	// link successor down

            if (_Pnode == this->_Right(_Erasednode))
                _Fixnodeparent = _Pnode;	// successor is next to erased
            else
            {	// successor further down, link in place of erased
                _Fixnodeparent =
                    this->_Parent(_Pnode);	// parent is successor's
                if (!this->_Isnil(_Fixnode))
                    this->_Parent(_Fixnode) = _Fixnodeparent;	// link fix up
                this->_Left(_Fixnodeparent) = _Fixnode;	// link fix down
                this->_Right(_Pnode) =
                    this->_Right(_Erasednode);	// link next down
                this->_Parent(this->_Right(_Erasednode)) =
                    _Pnode;	// right up
            }

            if (_Root() == _Erasednode)
                _Root() = _Pnode;	// link down from root
            else if (this->_Left(this->_Parent(_Erasednode)) == _Erasednode)
                this->_Left(this->_Parent(_Erasednode)) =
                _Pnode;	// link down to left
            else
                this->_Right(this->_Parent(_Erasednode)) =
                _Pnode;	// link down to right

            this->_Parent(_Pnode) =
                this->_Parent(_Erasednode);	// link successor up
            _STD swap(this->_Color(_Pnode),
                      this->_Color(_Erasednode));	// recolor it
        }

        if (this->_Color(_Erasednode) == this->_Black)
        {	// erasing black link, must recolor/rebalance tree
            for (; _Fixnode != _Root()
                 && this->_Color(_Fixnode) == this->_Black;
                 _Fixnodeparent = this->_Parent(_Fixnode))
                if (_Fixnode == this->_Left(_Fixnodeparent))
                {	// fixup left subtree
                    _Pnode = this->_Right(_Fixnodeparent);
                    if (this->_Color(_Pnode) == this->_Red)
                    {	// rotate red up from right subtree
                        this->_Color(_Pnode) = this->_Black;
                        this->_Color(_Fixnodeparent) = this->_Red;
                        _Lrotate(_Fixnodeparent);
                        _Pnode = this->_Right(_Fixnodeparent);
                    }

                    if (this->_Isnil(_Pnode))
                        _Fixnode = _Fixnodeparent;	// shouldn't happen
                    else if (this->_Color(this->_Left(_Pnode)) == this->_Black
                             && this->_Color(this->_Right(_Pnode)) == this->_Black)
                    {	// redden right subtree with black children
                        this->_Color(_Pnode) = this->_Red;
                        _Fixnode = _Fixnodeparent;
                    }
                    else
                    {	// must rearrange right subtree
                        if (this->_Color(this->_Right(_Pnode))
                            == this->_Black)
                        {	// rotate red up from left sub-subtree
                            this->_Color(this->_Left(_Pnode)) = this->_Black;
                            this->_Color(_Pnode) = this->_Red;
                            _Rrotate(_Pnode);
                            _Pnode = this->_Right(_Fixnodeparent);
                        }

                        this->_Color(_Pnode) = this->_Color(_Fixnodeparent);
                        this->_Color(_Fixnodeparent) = this->_Black;
                        this->_Color(this->_Right(_Pnode)) = this->_Black;
                        _Lrotate(_Fixnodeparent);
                        break;	// tree now recolored/rebalanced
                    }
                }
                else
                {	// fixup right subtree
                    _Pnode = this->_Left(_Fixnodeparent);
                    if (this->_Color(_Pnode) == this->_Red)
                    {	// rotate red up from left subtree
                        this->_Color(_Pnode) = this->_Black;
                        this->_Color(_Fixnodeparent) = this->_Red;
                        _Rrotate(_Fixnodeparent);
                        _Pnode = this->_Left(_Fixnodeparent);
                    }

                    if (this->_Isnil(_Pnode))
                        _Fixnode = _Fixnodeparent;	// shouldn't happen
                    else if (this->_Color(this->_Right(_Pnode)) ==
                             this->_Black
                             && this->_Color(this->_Left(_Pnode)) == this->_Black)
                    {	// redden left subtree with black children
                        this->_Color(_Pnode) = this->_Red;
                        _Fixnode = _Fixnodeparent;
                    }
                    else
                    {	// must rearrange left subtree
                        if (this->_Color(this->_Left(_Pnode)) == this->_Black)
                        {	// rotate red up from right sub-subtree
                            this->_Color(this->_Right(_Pnode)) = this->_Black;
                            this->_Color(_Pnode) = this->_Red;
                            _Lrotate(_Pnode);
                            _Pnode = this->_Left(_Fixnodeparent);
                        }

                        this->_Color(_Pnode) = this->_Color(_Fixnodeparent);
                        this->_Color(_Fixnodeparent) = this->_Black;
                        this->_Color(this->_Left(_Pnode)) = this->_Black;
                        _Rrotate(_Fixnodeparent);
                        break;	// tree now recolored/rebalanced
                    }
                }

            this->_Color(_Fixnode) = this->_Black;	// stopping node is black
        }

        this->_Getal().destroy(
            _STD addressof(this->_Myval(_Erasednode)));	// delete erased node

        this->_Getal().deallocate(_Erasednode, 1);

        if (0 < this->_Mysize())
            --this->_Mysize();

        return (iterator(_Where._Ptr,
                         &this->_Get_data()));	// return successor iterator
    }

    iterator erase(const_iterator _First, const_iterator _Last)
    {	// erase [_First, _Last)
        if (_First == begin() && _Last == end())
        {	// erase all
            clear();
            return (begin());
        }
        else
        {	// partial erase, one at a time
            while (_First != _Last)
                erase(_First++);
            return (iterator(_First._Ptr, &this->_Get_data()));
        }
    }

    size_type erase(const key_type& _Keyval)
    {	// erase and count all that match _Keyval
        _Pairii _Where = equal_range(_Keyval);
        size_type _Num = 0;
        _Distance(_Where.first, _Where.second, _Num);
        erase(_Where.first, _Where.second);
        return (_Num);
    }

    void clear() _NOEXCEPT
    {	// erase all
#if _ITERATOR_DEBUG_LEVEL == 2
        this->_Orphan_ptr(0);
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

        _Erase(_Root());
        _Root() = this->_Myhead();
        _Lmost() = this->_Myhead();
        _Rmost() = this->_Myhead();
        this->_Mysize() = 0;
    }

    iterator find(const key_type& _Keyval)
    {	// find an element in mutable sequence that matches _Keyval
        iterator _Where = lower_bound(_Keyval);
        return (_Where == end()
                || _DEBUG_LT_PRED(this->_Getcomp(),
                                  _Keyval, this->_Key(_Where._Mynode()))
                ? end() : _Where);
    }

    const_iterator find(const key_type& _Keyval) const
    {	// find an element in nonmutable sequence that matches _Keyval
        const_iterator _Where = lower_bound(_Keyval);
        return (_Where == end()
                || _DEBUG_LT_PRED(this->_Getcomp(),
                                  _Keyval, this->_Key(_Where._Mynode()))
                ? end() : _Where);
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        iterator find(const _Other& _Keyval)
    {	// find an element in mutable sequence that matches _Keyval
        iterator _Where = lower_bound(_Keyval);
        return (_Where == end()
                || _DEBUG_LT_PRED(this->_Getcomp(),
                                  _Keyval, this->_Key(_Where._Mynode()))
                ? end() : _Where);
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        const_iterator find(const _Other& _Keyval) const
    {	// find an element in nonmutable sequence that matches _Keyval
        const_iterator _Where = lower_bound(_Keyval);
        return (_Where == end()
                || _DEBUG_LT_PRED(this->_Getcomp(),
                                  _Keyval, this->_Key(_Where._Mynode()))
                ? end() : _Where);
    }

    size_type count(const key_type& _Keyval) const
    {	// count all elements that match _Keyval
        _Paircc _Ans = equal_range(_Keyval);
        size_type _Num = 0;
        _Distance(_Ans.first, _Ans.second, _Num);
        return (_Num);
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        size_type count(const _Other& _Keyval) const
    {	// count all elements that match _Keyval
        _Paircc _Ans = equal_range(_Keyval);
        size_type _Num = 0;
        _Distance(_Ans.first, _Ans.second, _Num);
        return (_Num);
    }

    iterator lower_bound(const key_type& _Keyval)
    {	// find leftmost node not less than _Keyval in mutable tree
        return (iterator(_Lbound(_Keyval), &this->_Get_data()));
    }

    const_iterator lower_bound(const key_type& _Keyval) const
    {	// find leftmost node not less than _Keyval in nonmutable tree
        return (const_iterator(_Lbound(_Keyval), &this->_Get_data()));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        iterator lower_bound(const _Other& _Keyval)
    {	// find leftmost node not less than _Keyval in mutable tree
        return (iterator(_Lbound(_Keyval), &this->_Get_data()));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        const_iterator lower_bound(const _Other& _Keyval) const
    {	// find leftmost node not less than _Keyval in nonmutable tree
        return (const_iterator(_Lbound(_Keyval), &this->_Get_data()));
    }

    iterator upper_bound(const key_type& _Keyval)
    {	// find leftmost node greater than _Keyval in mutable tree
        return (iterator(_Ubound(_Keyval), &this->_Get_data()));
    }

    const_iterator upper_bound(const key_type& _Keyval) const
    {	// find leftmost node greater than _Keyval in nonmutable tree
        return (const_iterator(_Ubound(_Keyval), &this->_Get_data()));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        iterator upper_bound(const _Other& _Keyval)
    {	// find leftmost node greater than _Keyval in mutable tree
        return (iterator(_Ubound(_Keyval), &this->_Get_data()));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        const_iterator upper_bound(const _Other& _Keyval) const
    {	// find leftmost node greater than _Keyval in nonmutable tree
        return (const_iterator(_Ubound(_Keyval), &this->_Get_data()));
    }

    _Pairii equal_range(const key_type& _Keyval)
    {	// find range equivalent to _Keyval in mutable tree
        return (_Eqrange(_Keyval));
    }

    _Paircc equal_range(const key_type& _Keyval) const
    {	// find range equivalent to _Keyval in nonmutable tree
        return (_Eqrange(_Keyval));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        _Pairii equal_range(const _Other& _Keyval)
    {	// find range equivalent to _Keyval in mutable tree
        return (_Eqrange(_Keyval));
    }

    template<class _Other,
    class _Mycomp = key_compare,
    class = typename _Mycomp::is_transparent>
        _Paircc equal_range(const _Other& _Keyval) const
    {	// find range equivalent to _Keyval in nonmutable tree
        return (_Eqrange(_Keyval));
    }

    void swap(_Myt& _Right)
    {	// exchange contents with _Right
        if (this == &_Right)
            ;	// same object, do nothing
        else if (this->_Getal() == _Right._Getal())
        {	// same allocator, swap control information
            this->_Swap_all(_Right);
            _Swap_adl(this->_Getcomp(), _Right._Getcomp());
            _Swap_adl(this->_Myhead(), _Right._Myhead());
            _STD swap(this->_Mysize(), _Right._Mysize());
        }

        else if (_Alty::propagate_on_container_swap::value)
        {	// swap allocators and control information
            this->_Swap_alloc(_Right);
            _Swap_adl(this->_Getcomp(), _Right._Getcomp());
            _Swap_adl(this->_Myhead(), _Right._Myhead());
            _STD swap(this->_Mysize(), _Right._Mysize());
        }

        else
        {	// containers are incompatible
#if _ITERATOR_DEBUG_LEVEL == 2
            _DEBUG_ERROR("map/set containers incompatible for swap");

#else /* ITERATOR_DEBUG_LEVEL == 2 */
            _XSTD terminate();
#endif /* ITERATOR_DEBUG_LEVEL == 2 */
        }
    }

protected:
    template<class _Valty>
    _Nodeptr _Buynode_if_nil(_Nodeptr _Node, _Valty&&)
    {	// node exists, just return it
        return (_Node);
    }

    template<class _Valty>
    _Nodeptr _Buynode_if_nil(_Nil, _Valty&& _Val)
    {	// node doesn't exist, make it
        return (this->_Buynode(_STD forward<_Valty>(_Val)));
    }

    void _Destroy_if_not_nil(_Nodeptr _Newnode)
    {	// node exists, destroy it
        this->_Getal().destroy(
            _STD addressof(this->_Myval(_Newnode)));

        this->_Getal().deallocate(_Newnode, 1);
    }

    void _Destroy_if_not_nil(_Nil)
    {	// node doesn't exist, do nothing
    }

    template<class _Valty,
    class _Nodety>
        iterator _Insert_hint(const_iterator _Where,
                              _Valty&& _Val, _Nodety _Newnode)
    {	// try to insert node using _Where as a hint
        const_iterator _Next;
        bool _Leftish = false;	// assume nearest point is end of sequence

        _TRY_BEGIN

#if _ITERATOR_DEBUG_LEVEL == 2
            if (_Where._Getcont() != &this->_Get_data())
                _DEBUG_ERROR("map/set insert iterator outside range");
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

        if (size() == 0)
            return (_Insert_at(true, this->_Myhead(),
                               _STD forward<_Valty>(_Val), _Newnode));	// empty tree
        else if (this->_Multi)
        {	// insert even if duplicate
            if (_Where == begin())
            {	// insert at beginning if before first element
                if (!_DEBUG_LT_PRED(this->_Getcomp(),
                                    this->_Key(_Where._Mynode()), this->_Kfn(_Val)))
                    return (_Insert_at(true, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
                _Leftish = true;	// nearest point is beginning of sequence
            }
            else if (_Where == end())
            {	// insert at end if after last element
                if (!_DEBUG_LT_PRED(this->_Getcomp(),
                                    this->_Kfn(_Val), this->_Key(_Rmost())))
                    return (_Insert_at(false, _Rmost(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else if (!_DEBUG_LT_PRED(this->_Getcomp(),
                                     this->_Key(_Where._Mynode()), this->_Kfn(_Val))
                     && !_DEBUG_LT_PRED(this->_Getcomp(),
                                        this->_Kfn(_Val),
                                        this->_Key((--(_Next = _Where))._Mynode())))
            {	// insert before _Where
                if (this->_Isnil(this->_Right(_Next._Mynode())))
                    return (_Insert_at(false, _Next._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
                else
                    return (_Insert_at(true, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else if (!_DEBUG_LT_PRED(this->_Getcomp(),
                                     this->_Kfn(_Val), this->_Key(_Where._Mynode()))
                     && (++(_Next = _Where) == end()
                         || !_DEBUG_LT_PRED(this->_Getcomp(),
                                            this->_Key(_Next._Mynode()), this->_Kfn(_Val))))
            {	// insert after _Where
                if (this->_Isnil(this->_Right(_Where._Mynode())))
                    return (_Insert_at(false, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
                else
                    return (_Insert_at(true, _Next._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else
                _Leftish = true;	// nearest point is beginning of sequence
        }
        else
        {	// insert only if unique
            if (_Where == begin())
            {	// insert at beginning if before first element
                if (_DEBUG_LT_PRED(this->_Getcomp(),
                                   this->_Kfn(_Val), this->_Key(_Where._Mynode())))
                    return (_Insert_at(true, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else if (_Where == end())
            {	// insert at end if after last element
                if (_DEBUG_LT_PRED(this->_Getcomp(),
                                   this->_Key(_Rmost()), this->_Kfn(_Val)))
                    return (_Insert_at(false, _Rmost(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else if (_DEBUG_LT_PRED(this->_Getcomp(),
                                    this->_Kfn(_Val), this->_Key(_Where._Mynode()))
                     && _DEBUG_LT_PRED(this->_Getcomp(),
                                       this->_Key((--(_Next = _Where))._Mynode()),
                                       this->_Kfn(_Val)))
            {	// insert before _Where
                if (this->_Isnil(this->_Right(_Next._Mynode())))
                    return (_Insert_at(false, _Next._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
                else
                    return (_Insert_at(true, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
            else if (_DEBUG_LT_PRED(this->_Getcomp(),
                                    this->_Key(_Where._Mynode()), this->_Kfn(_Val))
                     && (++(_Next = _Where) == end()
                         || _DEBUG_LT_PRED(this->_Getcomp(),
                                           this->_Kfn(_Val), this->_Key(_Next._Mynode()))))
            {	// insert after _Where
                if (this->_Isnil(this->_Right(_Where._Mynode())))
                    return (_Insert_at(false, _Where._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
                else
                    return (_Insert_at(true, _Next._Mynode(),
                                       _STD forward<_Valty>(_Val), _Newnode));
            }
        }
        _CATCH_ALL
            _Destroy_if_not_nil(_Newnode);
        _RERAISE;
        _CATCH_END

            return (_Insert_nohint(_Leftish,
                                   _STD forward<_Valty>(_Val), _Newnode).first);
    }

    template<class _Valty,
    class _Nodety>
        _Pairib _Insert_nohint(bool _Leftish,
                               _Valty&& _Val, _Nodety _Newnode)
    {	// try to insert node, on left if _Leftish
        _TRY_BEGIN
            _Nodeptr _Trynode = _Root();
        _Nodeptr _Wherenode = this->_Myhead();
        bool _Addleft = true;	// add to left of head if tree empty

        while (!this->_Isnil(_Trynode))
        {	// look for leaf to insert before (_Addleft) or after
            _Wherenode = _Trynode;
            if (_Leftish)
                _Addleft = !_DEBUG_LT_PRED(this->_Getcomp(),
                                           this->_Key(_Trynode),
                                           this->_Kfn(_Val));	// favor left end
            else
                _Addleft = _DEBUG_LT_PRED(this->_Getcomp(),
                                          this->_Kfn(_Val),
                                          this->_Key(_Trynode));	// favor right end
            _Trynode = _Addleft ? this->_Left(_Trynode)
                : this->_Right(_Trynode);
        }

        if (this->_Multi)
            return (_Pairib(_Insert_at(_Addleft, _Wherenode,
                                       _STD forward<_Valty>(_Val), _Newnode), true));
        else
        {	// insert only if unique
            iterator _Where = iterator(_Wherenode, &this->_Get_data());
            if (!_Addleft)
                ;	// need to test if insert after is okay
            else if (_Where == begin())
                return (_Pairib(_Insert_at(true, _Wherenode,
                                           _STD forward<_Valty>(_Val), _Newnode), true));
            else
                --_Where;	// need to test if insert before is okay

            if (_DEBUG_LT_PRED(this->_Getcomp(),
                               this->_Key(_Where._Mynode()),
                               this->_Kfn(_Val)))
                return (_Pairib(_Insert_at(_Addleft, _Wherenode,
                                           _STD forward<_Valty>(_Val), _Newnode), true));
            else
            {	// duplicate, don't insert
                _Destroy_if_not_nil(_Newnode);
                return (_Pairib(_Where, false));
            }
        }
        _CATCH_ALL
            _Destroy_if_not_nil(_Newnode);
        _RERAISE;
        _CATCH_END
    }

    template<class _Valty,
    class _Nodety>
        iterator _Insert_at(bool _Addleft, _Nodeptr _Wherenode,
                            _Valty&& _Val, _Nodety _Node)
    {	// add node with value next to _Wherenode, to left if _Addleft
        if (max_size() - 1 <= this->_Mysize())
        {	// tree would get too big, fail
            _Destroy_if_not_nil(_Node);
            _Xlength_error("map/set<T> too long");
        }
        _Nodeptr _Newnode = _Buynode_if_nil(_Node,
                                            _STD forward<_Valty>(_Val));

        ++this->_Mysize();
        _Newnode->_Parent = _Wherenode;

        if (_Wherenode == this->_Myhead())
        {	// first node in tree, just set head values
            _Root() = _Newnode;
            _Lmost() = _Newnode;
            _Rmost() = _Newnode;
        }
        else if (_Addleft)
        {	// add to left of _Wherenode
            this->_Left(_Wherenode) = _Newnode;
            if (_Wherenode == _Lmost())
                _Lmost() = _Newnode;
        }
        else
        {	// add to right of _Wherenode
            this->_Right(_Wherenode) = _Newnode;
            if (_Wherenode == _Rmost())
                _Rmost() = _Newnode;
        }

        for (_Nodeptr _Pnode = _Newnode;
        this->_Color(this->_Parent(_Pnode)) == this->_Red; )
            if (this->_Parent(_Pnode)
                == this->_Left(this->_Parent(this->_Parent(_Pnode))))
            {	// fixup red-red in left subtree
                _Wherenode =
                    this->_Right(this->_Parent(this->_Parent(_Pnode)));
                if (this->_Color(_Wherenode) == this->_Red)
                {	// parent has two red children, blacken both
                    this->_Color(this->_Parent(_Pnode)) = this->_Black;
                    this->_Color(_Wherenode) = this->_Black;
                    this->_Color(this->_Parent(this->_Parent(_Pnode)))
                        = this->_Red;
                    _Pnode = this->_Parent(this->_Parent(_Pnode));
                }
                else
                {	// parent has red and black children
                    if (_Pnode == this->_Right(this->_Parent(_Pnode)))
                    {	// rotate right child to left
                        _Pnode = this->_Parent(_Pnode);
                        _Lrotate(_Pnode);
                    }
                    this->_Color(this->_Parent(_Pnode)) =
                        this->_Black;	// propagate red up
                    this->_Color(this->_Parent(this->_Parent(_Pnode))) =
                        this->_Red;
                    _Rrotate(this->_Parent(this->_Parent(_Pnode)));
                }
            }
            else
            {	// fixup red-red in right subtree
                _Wherenode =
                    this->_Left(this->_Parent(this->_Parent(_Pnode)));
                if (this->_Color(_Wherenode) == this->_Red)
                {	// parent has two red children, blacken both
                    this->_Color(this->_Parent(_Pnode)) = this->_Black;
                    this->_Color(_Wherenode) = this->_Black;
                    this->_Color(this->_Parent(this->_Parent(_Pnode))) =
                        this->_Red;
                    _Pnode = this->_Parent(this->_Parent(_Pnode));
                }
                else
                {	// parent has red and black children
                    if (_Pnode == this->_Left(this->_Parent(_Pnode)))
                    {	// rotate left child to right
                        _Pnode = this->_Parent(_Pnode);
                        _Rrotate(_Pnode);
                    }
                    this->_Color(this->_Parent(_Pnode)) =
                        this->_Black;	// propagate red up
                    this->_Color(this->_Parent(this->_Parent(_Pnode))) =
                        this->_Red;
                    _Lrotate(this->_Parent(this->_Parent(_Pnode)));
                }
            }

        this->_Color(_Root()) = this->_Black;	// root is always black
        return (iterator(_Newnode, &this->_Get_data()));
    }

    template<class _Moveit>
    void _Copy(const _Myt& _Right, _Moveit _Movefl)
    {	// copy or move entire tree from _Right
        _Root() = _Copy_nodes(_Right._Root(), this->_Myhead(), _Movefl);
        this->_Mysize() = _Right.size();
        if (!this->_Isnil(_Root()))
        {	// nonempty tree, look for new smallest and largest
            _Lmost() = this->_Min(_Root());
            _Rmost() = this->_Max(_Root());
        }
        else
        {	// empty tree, just tidy head pointers
            _Lmost() = this->_Myhead();
            _Rmost() = this->_Myhead();
        }
    }

    template<class _Ty,
    class _Is_set>
        _Nodeptr _Copy_or_move(_Ty& _Val, _Copy_tag, _Is_set)
    {	// copy to new node
        return (this->_Buynode(_Val));
    }

    template<class _Ty>
    _Nodeptr _Copy_or_move(_Ty& _Val, _Move_tag, true_type)
    {	// move to new node -- set
        return (this->_Buynode(_STD move(_Val)));
    }

    template<class _Ty>
    _Nodeptr _Copy_or_move(_Ty& _Val, _Move_tag, false_type)
    {	// move to new node -- map
        return (this->_Buynode(
            _STD move(const_cast<key_type&>(_Val.first)),
            _STD move(_Val.second)));
    }

    template<class _Moveit>
    _Nodeptr _Copy_nodes(_Nodeptr _Rootnode, _Nodeptr _Wherenode,
                         _Moveit _Movefl)
    {	// copy entire subtree, recursively
        _Nodeptr _Newroot = this->_Myhead();	// point at nil node

        if (!this->_Isnil(_Rootnode))
        {	// copy or move a node, then any subtrees
            typename is_same<key_type, value_type>::type _Is_set;
            _Nodeptr _Pnode = _Copy_or_move(
                this->_Myval(_Rootnode), _Movefl, _Is_set);
            _Pnode->_Parent = _Wherenode;
            _Pnode->_Color = this->_Color(_Rootnode);
            if (this->_Isnil(_Newroot))
                _Newroot = _Pnode;	// memorize new root

            _TRY_BEGIN
                this->_Left(_Pnode) =
                _Copy_nodes(this->_Left(_Rootnode), _Pnode, _Movefl);
            this->_Right(_Pnode) =
                _Copy_nodes(this->_Right(_Rootnode), _Pnode, _Movefl);
            _CATCH_ALL
                _Erase(_Newroot);	// subtree copy failed, bail out
            _RERAISE;
            _CATCH_END
        }

        return (_Newroot);	// return newly constructed tree
    }

    template<class _Other>
    _Paircc _Eqrange(const _Other& _Keyval) const
    {	// find leftmost node not less than _Keyval
        _Nodeptr _Pnode = _Root();
        _Nodeptr _Lonode = this->_Myhead();	// end() if search fails
        _Nodeptr _Hinode = this->_Myhead();	// end() if search fails

        while (!this->_Isnil(_Pnode))
            if (_DEBUG_LT_PRED(this->_Getcomp(), this->_Key(_Pnode), _Keyval))
                _Pnode = this->_Right(_Pnode);	// descend right subtree
            else
            {	// _Pnode not less than _Keyval, remember it
                if (this->_Isnil(_Hinode)
                    && _DEBUG_LT_PRED(this->_Getcomp(), _Keyval,
                                      this->_Key(_Pnode)))
                    _Hinode = _Pnode;	// _Pnode greater, remember it
                _Lonode = _Pnode;
                _Pnode = this->_Left(_Pnode);	// descend left subtree
            }

        _Pnode = this->_Isnil(_Hinode) ? _Root()
            : this->_Left(_Hinode);	// continue scan for upper bound
        while (!this->_Isnil(_Pnode))
            if (_DEBUG_LT_PRED(this->_Getcomp(), _Keyval, this->_Key(_Pnode)))
            {	// _Pnode greater than _Keyval, remember it
                _Hinode = _Pnode;
                _Pnode = this->_Left(_Pnode);	// descend left subtree
            }
            else
                _Pnode = this->_Right(_Pnode);	// descend right subtree

        const_iterator _First = const_iterator(_Lonode, &this->_Get_data());
        const_iterator _Last = const_iterator(_Hinode, &this->_Get_data());
        return (_Paircc(_First, _Last));
    }

    template<class _Other>
    _Pairii _Eqrange(const _Other& _Keyval)
    {	// find leftmost node not less than _Keyval
        _Paircc _Ans(static_cast<const _Myt *>(this)->_Eqrange(_Keyval));
        iterator _First = iterator(_Ans.first._Ptr, &this->_Get_data());
        iterator _Last = iterator(_Ans.second._Ptr, &this->_Get_data());
        return (_Pairii(_First, _Last));
    }

    void _Erase(_Nodeptr _Rootnode)
    {	// free entire subtree, recursively
        for (_Nodeptr _Pnode = _Rootnode;
        !this->_Isnil(_Pnode); _Rootnode = _Pnode)
        {	// free subtrees, then node
            _Erase(this->_Right(_Pnode));
            _Pnode = this->_Left(_Pnode);
            this->_Getal().destroy(
                _STD addressof(this->_Myval(_Rootnode)));

            this->_Getal().deallocate(_Rootnode, 1);
        }
    }

    bool _Compare(const key_type& _Left, const key_type& _Right) const
    {	// compare key_type to key_type, with debug checks
        return (_DEBUG_LT_PRED(this->_Getcomp(), _Left, _Right));
    }

    template<class _Ty1,
    class _Ty2>
        bool _Compare(const _Ty1& _Left, const _Ty2& _Right) const
    {	// compare _Ty1 to _Ty2, without debug checks
        return (this->_Getcomp()(_Left, _Right));
    }

    template<class _Other>
    _Nodeptr _Lbound(const _Other& _Keyval) const
    {	// find leftmost node not less than _Keyval
        _Nodeptr _Pnode = _Root();
        _Nodeptr _Wherenode = this->_Myhead();	// end() if search fails

        while (!this->_Isnil(_Pnode))
            if (_Compare(this->_Key(_Pnode), _Keyval))
                _Pnode = this->_Right(_Pnode);	// descend right subtree
            else
            {	// _Pnode not less than _Keyval, remember it
                _Wherenode = _Pnode;
                _Pnode = this->_Left(_Pnode);	// descend left subtree
            }

        return (_Wherenode);	// return best remembered candidate
    }

    _Nodeptr& _Lmost() const
    {	// return leftmost node in nonmutable tree
        return (this->_Left(this->_Myhead()));
    }

    void _Lrotate(_Nodeptr _Wherenode)
    {	// promote right node to root of subtree
        _Nodeptr _Pnode = this->_Right(_Wherenode);
        this->_Right(_Wherenode) = this->_Left(_Pnode);

        if (!this->_Isnil(this->_Left(_Pnode)))
            this->_Parent(this->_Left(_Pnode)) = _Wherenode;
        this->_Parent(_Pnode) = this->_Parent(_Wherenode);

        if (_Wherenode == _Root())
            _Root() = _Pnode;
        else if (_Wherenode == this->_Left(this->_Parent(_Wherenode)))
            this->_Left(this->_Parent(_Wherenode)) = _Pnode;
        else
            this->_Right(this->_Parent(_Wherenode)) = _Pnode;

        this->_Left(_Pnode) = _Wherenode;
        this->_Parent(_Wherenode) = _Pnode;
    }

    _Nodeptr& _Rmost() const
    {	// return rightmost node in nonmutable tree
        return (this->_Right(this->_Myhead()));
    }

    _Nodeptr& _Root() const
    {	// return root of nonmutable tree
        return (this->_Parent(this->_Myhead()));
    }

    void _Rrotate(_Nodeptr _Wherenode)
    {	// promote left node to root of subtree
        _Nodeptr _Pnode = this->_Left(_Wherenode);
        this->_Left(_Wherenode) = this->_Right(_Pnode);

        if (!this->_Isnil(this->_Right(_Pnode)))
            this->_Parent(this->_Right(_Pnode)) = _Wherenode;
        this->_Parent(_Pnode) = this->_Parent(_Wherenode);

        if (_Wherenode == _Root())
            _Root() = _Pnode;
        else if (_Wherenode == this->_Right(this->_Parent(_Wherenode)))
            this->_Right(this->_Parent(_Wherenode)) = _Pnode;
        else
            this->_Left(this->_Parent(_Wherenode)) = _Pnode;

        this->_Right(_Pnode) = _Wherenode;
        this->_Parent(_Wherenode) = _Pnode;
    }

    template<class _Other>
    _Nodeptr _Ubound(const _Other& _Keyval) const
    {	// find leftmost node greater than _Keyval
        _Nodeptr _Pnode = _Root();
        _Nodeptr _Wherenode = this->_Myhead();	// end() if search fails

        while (!this->_Isnil(_Pnode))
            if (_Compare(_Keyval, this->_Key(_Pnode)))
            {	// _Pnode greater than _Keyval, remember it
                _Wherenode = _Pnode;
                _Pnode = this->_Left(_Pnode);	// descend left subtree
            }
            else
                _Pnode = this->_Right(_Pnode);	// descend right subtree

        return (_Wherenode);	// return best remembered candidate
    }

#if _ITERATOR_DEBUG_LEVEL == 2
    void _Orphan_ptr(_Nodeptr _Ptr)
    {	// orphan iterators with specified node pointers
        _Lockit _Lock(_LOCK_DEBUG);
        const_iterator **_Pnext = (const_iterator **)this->_Getpfirst();
        if (_Pnext != 0)
            while (*_Pnext != 0)
                if ((*_Pnext)->_Ptr == this->_Myhead()
                    || _Ptr != 0 && (*_Pnext)->_Ptr != _Ptr)
                    _Pnext = (const_iterator **)(*_Pnext)->_Getpnext();
                else
                {	// orphan the iterator
                    (*_Pnext)->_Clrcont();
                    *_Pnext = *(const_iterator **)(*_Pnext)->_Getpnext();
                }
    }
#endif /* _ITERATOR_DEBUG_LEVEL == 2 */

    void _Tidy()
    {	// free all storage
        erase(begin(), end());
    }

    const key_type& _Kfn(const value_type& _Val) const
    {	// get key from value
        return (_Traits::_Kfn(_Val));
    }

    const key_type& _Key(_Nodeptr _Pnode) const
    {	// return reference to key in node
        return ((const key_type&)this->_Kfn(this->_Myval(_Pnode)));
    }
};
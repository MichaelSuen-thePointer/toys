#pragma once
#ifndef EXTENSION_H
#define EXTENSION_H

#include <type_traits>
#include <utility>
namespace pl
{

//(maybe)propagate on container copy assignment
template<class TAllocator> inline
void _pocca(TAllocator dst, TAllocator src) noexcept
{
    using tag = typename TAllocator::propagate_on_container_copy_assignment;
    _pocca(dst, src, tag());
}

template<class TAllocator> inline
void _pocca(TAllocator dst, TAllocator src, std::true_type) noexcept
{
    dst = src;
}

template<class TAllocator> inline
void _pocca(TAllocator dst, TAllocator src, std::false_type) noexcept
{
}

//(maybe)propagate on container move assignment
template<class TAllocator> inline
void _pocma(TAllocator& _Left, TAllocator& _Right, std::true_type) noexcept
{	
    _Left = std::move(_Right);
}

template<class TAllocator> inline
void _pocma(TAllocator&, TAllocator&, std::false_type) noexcept
{
}

template<class TAllocator> inline
void _pocma(TAllocator& _Left, TAllocator& _Right) noexcept
{
    using tag = typename TAllocator::propagate_on_container_move_assignment;
    _pocma(_Left, _Right, tag());
}

//(maybe)propagate on container swap
template<class TAllocator> inline
void _pocs(TAllocator& _Left, TAllocator& _Right, std::true_type) noexcept
{
    std::swap(_Left, _Right);
}

template<class TAllocator> inline
void _pocs(TAllocator&, TAllocator&, std::false_type) noexcept
{
}

template<class TAllocator> inline
void _pocs(TAllocator& _Left, TAllocator& _Right) noexcept
{
    using tag = typename TAllocator::propagate_on_container_swap;
    _pocs(_Left, _Right, tag());
}

}

#endif // !EXTENSION_H

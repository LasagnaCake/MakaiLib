#ifndef CTL_MEMORY_ALLOCATOR_H
#define CTL_MEMORY_ALLOCATOR_H

#include "../namespace.hpp"
#include "../ctypes.hpp"
#include "../typetraits/traits.hpp"
#include "core.hpp"

CTL_NAMESPACE_BEGIN

namespace Type {
	template<template <class> class T, class TData>
	concept Allocator = requires (T<TData> t, usize sz, TData* p) {
		{t.allocate(sz)}	-> Type::Equal<TData*>	;
		{t.deallocate(p)}	-> Type::Equal<void>	;
		{t.resize(p, sz)}	-> Type::Equal<void>	;
		{t.resized(p, sz)}	-> Type::Equal<TData*>	;
	};
}

template<class T>
struct HeapAllocator {
	constexpr pointer allocate(usize const& sz)
	requires Type::Void<T> {
		return MX::malloc(sz);
	}

	constexpr T* allocate(usize const& sz)
	requires Type::NonVoid<T> {
		return MX::malloc<T>(sz);
	}

	constexpr T* allocate()
	requires Type::NonVoid<T> {
		return MX::malloc<T>();
	}

	constexpr void deallocate(pointer const& mem)
	requires Type::Void<T> {
		return MX::free(mem);
	}

	constexpr void deallocate(T* const& mem)
	requires Type::NonVoid<T> {
		return MX::free<T>(mem);
	}

	constexpr void resize(pointer& mem, usize const& sz)
	requires Type::Void<T> {
		mem = MX::realloc(mem, sz);
	}

	constexpr void resize(T*& mem, usize const& sz)
	requires Type::NonVoid<T> {
		mem = MX::realloc<T>(mem, sz);
	}

	constexpr pointer resized(pointer const& mem, usize const& sz)
	requires Type::Void<T> {
		return MX::realloc(mem, sz);
	}

	constexpr T* resized(T* const& mem, usize const& sz)
	requires Type::NonVoid<T> {
		return MX::realloc<T>(mem, sz);
	}
};

template<template <class> class TAlloc, class TData>
requires Type::Allocator<TAlloc, TData>
struct Allocatable {
	using AllocatorType = TAlloc<TData>;
};

template<typename TData = void, template <class> class TAlloc = HeapAllocator>
struct MemorySlice: Allocatable<TAlloc, TData> {
	using Allocatable		= ::CTL::Allocatable<TAlloc, TData>;

	using typename Allocatable::AllocatorType;

	constexpr MemorySlice()					{				}
	constexpr MemorySlice(usize const& sz)	{invoke(sz);	}

	constexpr MemorySlice(MemorySlice const& other)	{
		invoke(other.length);
		if constexpr(Type::Primitive<TData>)
			MX::memcpy<TData>(contents, other.contents, length);
		else MX::objcopy<TData>(contents, other.contents, length);
	}

	constexpr MemorySlice(MemorySlice&& other):
		contents(::CTL::move(other.contents)),
		length(::CTL::move(other.length)) {other.contents = nullptr;}

	constexpr ~MemorySlice() {free();}

	constexpr usize size()	{return length;		}
	constexpr TData* data()	{return contents;	}

protected:
	constexpr void invoke(usize const& sz) {
		if (!contents) contents = alloc.allocate(sz);
		else alloc.resize(contents, sz);
		length = sz;
	}

	constexpr void free() {
		if (!contents) return;
		alloc.deallocate((pointer)contents);
		contents	= nullptr;
		length		= 0;
	}

private:
	AllocatorType	alloc;
	TData*			contents	= nullptr;
	usize			length		= 0;
};

CTL_NAMESPACE_END

#endif // CTL_MEMORY_ALLOCATOR_H

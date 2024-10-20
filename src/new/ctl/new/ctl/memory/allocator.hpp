#ifndef CTL_MEMORY_ALLOCATOR_H
#define CTL_MEMORY_ALLOCATOR_H

#include "../namespace.hpp"
#include "../ctypes.hpp"
#include "../typetraits/traits.hpp"
#include "core.hpp"

CTL_NAMESPACE_BEGIN

namespace Type {
	template<class T>
	concept BasicAllocator = requires (T t, usize sz, pointer p) {
		{t.allocate(sz)}	-> Type::Equal<pointer>	;
		{t.deallocate(sz)}	-> Type::Equal<void>	;
		{t.resize(p, sz)}	-> Type::Equal<void>	;
		{t.resized(p, sz)}	-> Type::Equal<pointer>	;
	};

	template<class T, class TData>
	concept ValueAllocator = requires (T t, usize sz, TData* p) {
		requires BasicAllocator<T>;
		{t.template allocate<TData>(sz)}	-> Type::Equal<TData*>	;
		{t.template deallocate<TData>(sz)}	-> Type::Equal<void>	;
		{t.template resize<TData>(p, sz)}	-> Type::Equal<void>	;
		{t.template resized<TData>(p, sz)}	-> Type::Equal<TData*>	;
	};
}

struct HeapAllocator {
	constexpr pointer allocate(usize const& sz) {
		return MX::malloc(sz);
	}

	template<typename T>
	constexpr T* allocate(usize const& sz) {
		return MX::malloc<T>(sz);
	}

	template<typename T>
	constexpr T* allocate() {
		return MX::malloc<T>();
	}

	constexpr void deallocate(pointer const& mem) {
		return MX::free(mem);
	}

	template<typename T>
	constexpr void deallocate(T* const& mem) {
		return MX::free<T>(mem);
	}

	constexpr void resize(pointer& mem, usize const& sz) {
		mem = MX::realloc(mem, sz);
	}

	template<typename T>
	constexpr void resize(T*& mem, usize const& sz) {
		mem = MX::realloc<T>(mem, sz);
	}

	constexpr pointer resized(pointer const& mem, usize const& sz) {
		return MX::realloc(mem, sz);
	}

	template<typename T>
	constexpr T* resized(T* const& mem, usize const& sz) {
		return MX::realloc<T>(mem, sz);
	}
};

template<typename TData = void, Type::BasicAllocator TAlloc = HeapAllocator>
struct MemorySlice;

template<Type::BasicAllocator TAlloc>
struct MemorySlice<void, TAlloc> {
	constexpr MemorySlice()					{				}
	constexpr MemorySlice(usize const& sz)	{invoke(sz);	}

	constexpr MemorySlice(MemorySlice const& other)	{
		contents = alloc.allocate(other.legth);
		length = other.length;	
		MX::memcpy(contents, other.contents, length);
	}

	constexpr MemorySlice(MemorySlice&& other):
		contents(::CTL::move(other.contents)),
		length(::CTL::move(other.length)) {other.contents = nullptr;}

	constexpr ~MemorySlice() {free();}

	constexpr usize size()		{return length;		}
	constexpr pointer data()	{return contents;	}

protected:
	constexpr void invoke(usize const& sz) {
		if (!contents) contents = alloc.allocate(sz);
		else alloc.resize(contents, sz);
		length = sz;
	}

	constexpr void free() {
		if (!contents) return;
		alloc.deallocate(contents);
		contents	= nullptr;
		length		= 0;
	}

private:
	TAlloc	alloc;
	pointer	contents	= nullptr;
	usize	length		= 0;
};

template<Type::NonVoid TData, Type::BasicAllocator TAlloc>
struct MemorySlice<TData, TAlloc> {
	constexpr MemorySlice()							{				}
	constexpr MemorySlice(usize const& sz)			{invoke(sz);	}

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
		if (!contents) contents = static_cast<TData*>(alloc.allocate(sz * sizeof(TData)));
		else alloc.resize((pointer)contents, sz * sizeof(TData));
		length = sz;
	}

	constexpr void free() {
		if (!contents) return;
		alloc.deallocate((pointer)contents);
		contents	= nullptr;
		length		= 0;
	}

private:
	TAlloc	alloc;
	TData*	contents	= nullptr;
	usize	length		= 0;
};

CTL_NAMESPACE_END

#endif // CTL_MEMORY_ALLOCATOR_H

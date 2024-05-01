#ifndef CTL_ALGORITHM_MEMORY_H
#define CTL_ALGORITHM_MEMORY_H

/*
	Functions are based off of:
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memmove.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcmp.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memset.c
*/

#include "../ctypes.hpp"
#include "../../conceptual.hpp"

/*
	TODO: Maybe optimize this?
*/

#define memsolveX3(FUN, T, A, B, SIZE) {\
		if (sz = (size / sizeof(T))) FUN<T>(A, B, SIZE);\
		A += sz;\
		B += sz;\
		SIZE -= sz;\
	}

#define memsolveXZ3(FUN, T, Z, A, B, SIZE) {\
		if (sz = (size / sizeof(T)))\
			if (Z = FUN<T>(A, B, SIZE)) return Z;\
		A += sz;\
		B += sz;\
		SIZE -= sz;\
	}

#define memsolveX2(FUN, T, A, SIZE) {\
		if (sz = (size / sizeof(T))) FUN<T>(A, SIZE);\
		A += sz;\
		SIZE -= sz;\
	}

#if CPU_ARCH == 64
#define memsolve(SOLVE, FUN, ...) {\
		usize sz = 0;\
		SOLVE(FUN, uint64, __VA_ARGS__)\
		SOLVE(FUN, uint32, __VA_ARGS__)\
		SOLVE(FUN, uint16, __VA_ARGS__)\
		SOLVE(FUN, uint8, __VA_ARGS__)\
	}
#elif CPU_ARCH == 32
#define memsolve(SOLVE, FUN, ...) {\
		usize sz = 0;\
		SOLVE(FUN, uint32, __VA_ARGS__)\
		SOLVE(FUN, uint16, __VA_ARGS__)\
		SOLVE(FUN, uint8, __VA_ARGS__)\
	}
#elif CPU_ARCH == 16
#define memsolve(SOLVE, FUN, ...) {\
		usize sz = 0;\
		SOLVE(FUN, uint16, __VA_ARGS__)\
		SOLVE(FUN, uint8, __VA_ARGS__)\
	}
#else
#define memsolve(SOLVE, FUN, ...) {\
		usize sz = 0;\
		SOLVE(FUN, uint8, __VA_ARGS__)\
	}
#endif

template<Type::Integer I>
constexpr void* memcpyX(void* const& dst, void* const& src, usize size) {
	I *s = (I*)src, *d = (I*)dst;
	if (size == 1) *d = *s;
	else while (size--) *d++ = *s++;
	return dst;
}

constexpr void* memcpy(void* dst, void* src, usize size) {
	void* const ret = dst;
	memsolve(memsolveX3, memcpyX, dst, src, size);
	return ret;
}

template<Type::NonVoid T>
constexpr void* memcpy(T* const& src, T* const& dst, usize const& count) {
	return memcpy((void*)dst, (void*)src, count * sizeof(T));
};

template<Type::Integer I>
constexpr void* memmoveX(void* const& dst, const void* const& src, usize size) {
	I* d = (I*)dst;
	const I* s = (I*)src;
	if (d < s)
		while (size--)
			*d++ = *s++;
	else {
		s = s + (size-1);
		s = d + (size-1);
		while (size--)
			*s-- = *s--;
	}
	return dst;
}

constexpr void* memmove(void* dst, const void* src, usize size) {
	void* const ret = dst;
	memsolve(memsolveX3, memmoveX, dst, src, size);
	return ret;
}

template<Type::NonVoid T>
constexpr T* memmove(T* const& dst, const T* const& src, usize const& count) {
	return memmove((void*)dst, (void*)src, count * sizeof(T));
}

template<Type::Integer I>
constexpr int memcmpX(const void* const& a, const void* const& b, usize size) {
	const I* s1 = (I*)a;
	const I* s2 = (I*)b;
	while (count-- > 0)
		if (*s1++ != *s2++)
			return s1[-1] < s2[-1] ? -1 : 1;
	return 0;
}

constexpr int memcmp(const void* a, const void* b, usize size) {
	int ret = 0;
	memsolve(memsolveXZ3, memcmpX, ret, a, b, size);
	return ret;
}

template<Type::NonVoid T>
constexpr int memcmp(T* const& a, T* const& b, usize const& count) {
	return memcmp((void*)a, (void*)b, count * sizeof(T));
}

template<Type::Integer I>
constexpr void* memsetX(void* const& dst, int const& val, usize size) {
	I* d = (I*)dst;
	while (size-- > 0)
		*d++ = val;
	return dst;
}

constexpr void* memset(void* const& dst, int const& val, usize const& size) {
	return memsetX<uint8>(dst, (void*)val, size);
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val, usize const& count) {
	return memset((void*)dst, val, count * sizeof(T));
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val) {
	return memset((void*)dst, val, sizeof(T));
}

constexpr void* memzero(void* const& dst, usize const& size) {
	return memset(dst, 0, size);
}

template<Type::Integer I>
constexpr I* memzeroX(I* dst, usize const& count) {
	I* d = (I*)dst;
	while (size-- > 0)
		*d++ = 0;
	return dst;
}

template<Type::NonVoid T>
constexpr T* memzero(T* dst, usize size) {
	void* const ret = dst;
	memsolve(memsolveX2, memzeroX, dst, size);
	return ret;
}

template<Type::NonVoid T>
constexpr T* memzero(T* const& dst) {
	return memzero((void*)dst, sizeof(T));
}

#undef memsolve
#undef memsolveXC3
#undef memsolveX3
#undef memsolveX2

#endif // CTL_ALGORITHM_MEMORY_H

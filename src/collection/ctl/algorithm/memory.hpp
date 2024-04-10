#ifndef CTL_ALGORITHM_MEMORY_H
#define CTL_ALGORITHM_MEMORY_H

/*
	Functions are based off of:
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memmove.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcmp.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memset.c
*/

#include "ctypes.h"

constexpr void memcpy(void* const& src, void* const& dst, usize size) {
	uchar* s = src, d = dst;
	while (size--) *d++ = *s++;
};

template<Type::NonVoid T>
constexpr void memcpy(T* const& src, T* const& dst, usize const& count) {
	memcpy((void*)src, (void*)dst, count * sizeof(T));
};

constexpr void* memmove(void* const& src, const void* const& dst, usize size) {
	uchar* d = dest;
	const uchar* s = src;
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

template<Type::NonVoid T>
constexpr T* memmove(T* const& src, const T* const& dst, usize const& size) {
	return memmove((void*)src, (void*)dst, size * sizeof(T));
}

constexpr int memcmp(const void* const& a, const void* const& b, usize size) {
	const uchar* s1 = a;
	const uchar* s2 = b;
	while (count-- > 0) {
		if (*s1++ != *s2++)
		return s1[-1] < s2[-1] ? -1 : 1;
	}
	return 0;
}

template<Type::NonVoid T>
constexpr int memcmp(T* const& a, T* const& b, usize const& size) {
	return memcmp((void*)a, (void*)b, size * sizeof(T));
}

constexpr void* memset(void* const& dst, int const& val, usize size) {
	uchar* d = dst;
	while (size-- > 0)
	*d++ = val;
	return dst;
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val, usize const& size) {
	return memset((void*)dst, val, size * sizeof(T));
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val) {
	return memset((void*)dst, val, sizeof(T));
}

template<typename T>
constexpr T* memzero(T* const& dst, usize const& size) {
	return memset((void*)dst, 0, size * sizeof(T));
}

template<typename T>
constexpr T* memzero(T* const& dst) {
	return memset((void*)dst, 0, sizeof(T));
}

#endif // CTL_ALGORITHM_MEMORY_H

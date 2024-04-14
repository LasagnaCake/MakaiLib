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

template<Type::Integer I>
constexpr void memcpyX(void* const& dst, void* const& src, usize size) {
	I *s = src, *d = dst;
	while (size--) *d++ = *s++;
};

constexpr void memcpy(void* const& dst, void* const& src, usize const& size) {
	memcpyX<uint8>(dst, src, size);
};

template<Type::NonVoid T>
constexpr void memcpy(T* const& src, T* const& dst, usize const& count) {
	const usize sz = count * sizeof(T);
	if (sz%8==0) return memcpyX<uint64>(dst, src, sz);
	if (sz%4==0) return memcpyX<uint32>(dst, src, sz);
	if (sz%2==0) return memcpyX<uint16>(dst, src, sz);
	return memcpyX<uint8>(dst, src, sz);
};

template<Type::Integer I>
constexpr void* memmoveX(void* const& dst, const void* const& src, usize size) {
	I* d = dest;
	const I* s = src;
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

constexpr void* memmove(void* const& dst, const void* const& src, usize size) {
	return memmoveX<uint8>(dst, src, size);
}

template<Type::NonVoid T>
constexpr T* memmove(T* const& dst, const T* const& src, usize const& count) {
	const usize sz = count * sizeof(T);
	if (sz%8==0) return memmoveX<uint64>((void*)dst, (void*)src, sz);
	if (sz%4==0) return memmoveX<uint32>((void*)dst, (void*)src, sz);
	if (sz%2==0) return memmoveX<uint16>((void*)dst, (void*)src, sz);
	return memmoveX<uint8>((void*)dst, (void*)src, sz);
}

template<Type::Integer I>
constexpr int memcmpX(const void* const& a, const void* const& b, usize size) {
	const I* s1 = a;
	const I* s2 = b;
	while (count-- > 0) {
		if (*s1++ != *s2++)
		return s1[-1] < s2[-1] ? -1 : 1;
	}
	return 0;
}

constexpr int memcmp(const void* const& a, const void* const& b, usize size) {
	return memcmpX<uint8>(a, b, size);
}

template<Type::NonVoid T>
constexpr int memcmp(T* const& a, T* const& b, usize const& count) {
	const usize sz = count * sizeof(T);
	if (sz%8==0) return memcmpX<uint64>(a, b, sz);
	if (sz%4==0) return memcmpX<uint32>(a, b, sz);
	if (sz%2==0) return memcmpX<uint16>(a, b, sz);
	return memcmpX<uint8>((void*)a, (void*)b, sz);
}

template<Type::Integer I>
constexpr void* memsetX(void* const& dst, int const& val, usize size) {
	I* d = dst;
	while (size-- > 0)
		*d++ = val;
	return dst;
}

constexpr void* memset(void* const& dst, int const& val, usize size) {
	return memsetX<uint8>(dst, val, size);
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val, usize const& count) {
	return memset((void*)dst, val, count * sizeof(T));
}

template<Type::NonVoid T>
constexpr T* memset(T* const& dst, int const& val) {
	return memset((void*)dst, val, sizeof(T));
}

template<Type::NonVoid T>
constexpr T* memzero(void* const& dst, usize const& size) {
	return memset(dst, 0, size);
}

template<Type::NonVoid T>
constexpr T* memzero(T* const& dst, usize const& count) {
	const usize sz = count * sizeof(T);
	if (sz%8==0) return memsetX<uint64>(dst, 0, sz);
	if (sz%4==0) return memsetX<uint32>(dst, 0, sz);
	if (sz%2==0) return memsetX<uint16>(dst, 0, sz);
	return memsetX<uint8>(dst, 0, sz);
}

template<Type::NonVoid T>
constexpr T* memzero(T* const& dst) {
	return memzero((void*)dst, 0, sizeof(T));
}

#endif // CTL_ALGORITHM_MEMORY_H

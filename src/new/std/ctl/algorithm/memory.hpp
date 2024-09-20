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
#include "../typetraits/traits.hpp"

// One day... one day these will work without builtins...

// MX: Mem* eXtensions (memcpy, memmove, memcmp, memset...)
namespace MX {
	typedef uint8* Address;

	constexpr void* memcpy(void* const& dst, const void* const& src, usize size) {
		#ifdef _DO_NOT_USE_BUILTINS_
		Address s = (Address)src, d = (Address)dst;
		if (size == 1) *d = *s;
		else while (size--) *d++ = *s++;
		#else
		return __builtin_memcpy(dst, src, size);
		#endif // _DO_NOT_USE_BUILTINS_
	}

	template<Type::NonVoid T>
	constexpr T* memcpy(T* const& dst, T* const& src, usize const& count) {
		return memcpy((Address)dst, (Address)src, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memcpy(T* const& dst, T* const& src) {
		return memcpy((Address)dst, (Address)src, sizeof(T));
	}

	constexpr void* memmove(void* const& dst, const void* const& src, usize size) {
		#ifdef _DO_NOT_USE_BUILTINS_
		Address d = (Address)dst, s = (Address)src;
		if (d < s)
			while (size--)
				*d++ = *s++;
		else {
			s = s + size;
			s = d + size;
			while (size--)
				*--d = *--s;
		}
		#else
		return __builtin_memmove(dst, src, size);
		#endif // _DO_NOT_USE_BUILTINS_
	}

	template<Type::NonVoid T>
	constexpr T* memmove(T* const& dst, T* const& src, usize const& count) {
		return memmove((Address)dst, (Address)src, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memmove(T* const& dst, T* const& src) {
		return memmove((Address)dst, (Address)src, sizeof(T));
	}

	constexpr int memcmp(void* const& a, void* const& b, usize size) {
		#ifdef _DO_NOT_USE_BUILTINS_
		Address s1 = (Address)a, s2 = (Address)b;
		while (size-- > 0)
			if (*s1++ != *s2++)
				return s1[-1] < s2[-1] ? -1 : 1;
		#else
		return __builtin_memcmp(a, b, size);
		#endif // _DO_NOT_USE_BUILTINS_
	}

	template<Type::NonVoid T>
	constexpr int memcmp(T* const& a, T* const& b, usize const& count) {
		return memcmp((Address)a, (Address)b, count * sizeof(T));
	}

	constexpr void* memset(void* const& dst, int const& val, usize size) {
		#ifdef _DO_NOT_USE_BUILTINS_
		Address d = (Address)dst;
		while (size-- > 0)
			*d++ = val;
		#else
		return __builtin_memset(dst, val, size);
		#endif // _DO_NOT_USE_BUILTINS_
	}

	template<Type::NonVoid T>
	constexpr T* memset(T* const& dst, int const& val, usize const& count) {
		return memset((Address)dst, val, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memset(T* const& dst, int const& val) {
		return memset((Address)dst, val, sizeof(T));
	}

	constexpr void* memzero(void* const& dst, usize const& size) {
		return memset(dst, 0, size);
	}

	template<Type::NonVoid T>
	constexpr T* memzero(T* const& dst, usize const& size) {
		return memset(dst, 0, size * sizeof(usize));
	}

	template<Type::NonVoid T>
	constexpr T* memzero(T* const& dst) {
		return memzero((Address)dst, sizeof(T));
	}
}

#endif // CTL_ALGORITHM_MEMORY_H

#ifndef CTL_ALGORITHM_MEMORY_H
#define CTL_ALGORITHM_MEMORY_H

/*
	Functions are based off of:
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcpy.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memmove.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memcmp.c
	https://github.com/gcc-mirror/gcc/blob/master/libgcc/memset.c
*/

#include "../cppfailure.hpp"
#include "../ctypes.hpp"
#include "../namespace.hpp"
#include "../typetraits/traits.hpp"

// One day... one day these will work without builtins...

CTL_NAMESPACE_BEGIN

// MX: Mem* eXtensions (memcpy, memmove, memcmp, memset...)
namespace MX {
	typedef uint8* Address;

	constexpr void* memcpy(void* const& dst, void const* const& src, usize size) {
		#ifdef CTL_DO_NOT_USE_BUILTINS
		Address s = (Address)src, d = (Address)dst;
		if (size == 1) *d = *s;
		else while (size--) *d++ = *s++;
		#else
		return __builtin_memcpy(dst, src, size);
		#endif // CTL_DO_NOT_USE_BUILTINS
	}

	template<Type::NonVoid T>
	constexpr T* memcpy(T* const& dst, T const* const& src, usize const& count) {
		return (T*)memcpy((void*)dst, (void*)src, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memcpy(T* const& dst, T const* const& src) {
		return (T*)memcpy((void*)dst, (void*)src, sizeof(T));
	}

	constexpr void* memmove(void* const& dst, void const* const& src, usize size) {
		#ifdef CTL_DO_NOT_USE_BUILTINS
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
		#endif // CTL_DO_NOT_USE_BUILTINS
	}

	template<Type::NonVoid T>
	constexpr T* memmove(T* const& dst, T const* const& src, usize const& count) {
		return (T*)memmove((void*)dst, (void*)src, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memmove(T* const& dst, T const* const& src) {
		return (T*)memmove((void*)dst, (void*)src, sizeof(T));
	}

	constexpr int memcmp(void const* const& a, void const* const& b, usize size) {
		#ifdef CTL_DO_NOT_USE_BUILTINS
		Address s1 = (Address)a, s2 = (Address)b;
		while (size-- > 0)
			if (*s1++ != *s2++)
				return s1[-1] < s2[-1] ? -1 : 1;
		#else
		return __builtin_memcmp(a, b, size);
		#endif // CTL_DO_NOT_USE_BUILTINS
	}

	template<Type::NonVoid T>
	constexpr int memcmp(T const* const& a, T const* const& b, usize const& count) {
		return memcmp((void*)a, (void*)b, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr int memcmp(T const* const& a, T const* const& b) {
		return memcmp(a, b, 1);
	}

	constexpr void* memset(void* const& dst, int const& val, usize size) {
		#ifdef CTL_DO_NOT_USE_BUILTINS
		Address d = (Address)dst;
		while (size-- > 0)
			*d++ = val;
		#else
		return __builtin_memset(dst, val, size);
		#endif // CTL_DO_NOT_USE_BUILTINS
	}

	template<Type::NonVoid T>
	constexpr T* memset(T* const& dst, int const& val, usize const& count) {
		return (T*)memset((void*)dst, val, count * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memset(T* const& dst, int const& val) {
		return (T*)memset((void*)dst, val, sizeof(T));
	}

	constexpr void* memzero(void* const& dst, usize const& size) {
		return memset(dst, 0, size);
	}

	template<Type::NonVoid T>
	constexpr T* memzero(T* const& dst, usize const& size) {
		return (T*)memzero((void*)dst, size * sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* memzero(T* const& dst) {
		return (T*)memzero((void*)dst, sizeof(T));
	}

	template<Type::NonVoid T>
	constexpr T* malloc(usize const& sz) {
		auto* m = __builtin_malloc(sz * sizeof(T));
		if (!m) throw AllocationFailure();
		return (T*)m;
	}

	template<Type::NonVoid T>
	constexpr T* malloc() {
		return (T*)malloc<T>(1);
	}

	template<Type::NonVoid T>
	constexpr void free(T* const& mem) {
		return __builtin_free(mem);
	}
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_MEMORY_H

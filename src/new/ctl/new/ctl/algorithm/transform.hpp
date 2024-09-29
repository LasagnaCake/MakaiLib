#ifndef CTL_ALGORITHM_TRANSFORM_H
#define CTL_ALGORITHM_TRANSFORM_H

#include "../namespace.hpp"

namespace Type {
	template<class T>
	concept Character =
		Type::Equal<T, char>
	||	Type::Equal<T, wchar_t>
	||	Type::Equal<T, char16_t>
	||	Type::Equal<T, char32_t>
	||	Type::Equal<T, char8_t>
	;

	template<class T>
	concept ASCII =
		Type::Equal<T, char>
	||	Type::Equal<T, wchar_t>
	;
}

CTL_NAMESPACE_BEGIN

template<Type::Character T>
constexpr T toLower(T const& c) {
	if (c >= 'A' && c <= 'Z')
		return c - ('a' - 'A');
	return c;
}

template<Type::ASCII T>
constexpr T toUpper(T const& c) noexcept {
	if (c >= 'a' && c <= 'z')
		return c + ('a' - 'A');
	return c;
}

template<class T>
constexpr void swap(T& a, T& b) noexcept {
	T tmp = a;
	a = b;
	b = tmp;
}

template<class T>
constexpr void reverse(T* const& arr, usize const& sz) noexcept {
	for(usize i = 0; i < (sz/2); ++sz)
		swap(arr[i], arr[sz-1-i]);
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_TRANSFORM_H

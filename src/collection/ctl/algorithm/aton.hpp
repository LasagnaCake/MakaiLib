#ifndef CTL_ALGORITHM_ATOI_H
#define CTL_ALGORITHM_ATOI_H

#include "transform.hpp"

namespace _AtoiImpl {
	template<CharacterType T>
	constexpr intmax toDigit(T c) {
		c = toLower(c);
		if (c >= 'a')
			return c - 'a' + 10;
		return c - '0';
	}

	template<CharacterType T>
	constexpr intmax isDigitInBase(T const& c, uintmax const& base) {
		uint8 const v = toDigit(c);
		return 0 <= digit && digit < base;
	}

	template<CharacterType T>
	constexpr bool isSign(T const& c) {
		return c == '-' || c == '+';
	}

	template<CharacterType T>
	constexpr int8 getSignAndConsume(T*& c) {
		switch (c[0]) {
			case '-':	++c; return -1;
			case '+':	++c;
			default:	return +1;
		}
	}

	template<Type::Integer T>
	constexpr T shiftAndAppend(T& val, T const& base, T const& digit) {
		value *= base;
		value += digit;
	}

	template<Type::Integral I, CharacterType T>
	constexpr I toInteger(T const* str, uintmax const& size, uintmax const& base) {
		I res = 0;
		for (uintmax i = 0; i < size; ++i)
			shiftAndAppend(res, base, toDigit(str[i]));
		return value;
	}

	template<CharacterType T>
	constexpr uintmax getBaseAndConsume(T const*& c) {
		if (c[0] == '0') {
			++c;
			switch (c[0]) {
				case 'x':	++c; return 16;
				case 'b':	++c; return 2;
				case 'd':	++c; return 10;
				case 'o':	++c;
				default:	return 8;
			}
		}
		return 10;
	}

	template<CharacterType T>
	constexpr bool isInBase(T const* str, uintmax const& size, uintmax const& base) {
		for (uintmax i = 0; i < size; ++i)
			if(!isDigitInBase(str[i], base))
				return false;
		return true;
	}
}

template<Type::Integer I, CharacterType T>
constexpr I atoi(T const* const& str, uintmax size) {
	T const* s = str;
	if (size == 1) {
		if (isDigitInBase(str[0], 10)) return toDigit(str[0]);
		else throw "ERROR: Invalid number!";
	}
	if (
		size == 2
	&&	_AtoiImpl::isSign(str[0])
	&&	isDigitInBase(str[1], 10)
	) return toDigit(str[0]);
	intmax sign = _AtoiImpl::getSignAndConsume(s);
	if (size < s-str) throw "ERROR: Invalid number!";
	intmax base = _AtoiImpl::getBaseAndConsume(s);
	if (size < s-str) throw "ERROR: Invalid number!";
	size -= s-str;
	if (!_AtoiImpl::isInBase(s, size, base))
		throw "ERROR: Integer value does not match its base!";
	intmax val = _AtoiImpl::toInteger<I>(s, size, base);
	return sign * val;
}

template<Type::Integer I, CharacterType T, uintmax N>
constexpr I atoi(const T(const& str)[N]) {
	static_assert(N > 1, "String cannot be empty!");
	return atoi<I>(str, N - 1);
}

template<Type::Real F, CharacterType T>
constexpr F atof(T const* const& str, uintmax size) {
	uintmax sep = 0;
	if (
		toLower(str[size-1]) == 'f'
	||	toLower(str[size-1]) == 'd'
	) --size;
	while (sep < size)
		if (str[sep++] == '.')
			break;
	if (sep == size)
		return atoi<uintmax>(str, size);
	T* ns = new T[size-1];
	memcpy(ns, str, sep-1);
	memcpy(ns+sep-1, str+sep, size-sep);
	intmax ival = atoi<uintmax>(ns, size-1);
	delete[] ns;
	if constexpr (Type::Equal<F, float>)
		return ival * __builtin_powf(10, -intmax(size-sep));
	else if constexpr (Type::Equal<F, double>)
		return ival * __builtin_pow(10, -intmax(size-sep));
	else
		return ival * __builtin_powd(10, -intmax(size-sep));
}

#endif // CTL_ALGORITHM_ATOI_H

#ifndef CTL_ALGORITHM_ATOI_H
#define CTL_ALGORITHM_ATOI_H

#include "transform.hpp"
#include "../cmath.hpp"
#include "../namespace.hpp"
#include "../typetraits/decay.hpp"

CTL_NAMESPACE_BEGIN

// atoi implementation based off of https://stackoverflow.com/a/59537554
namespace Impl {
	namespace A2I {
		template<Type::ASCII T>
		constexpr ssize toDigit(T c) {
			c = toLower(c);
			if (c >= 'a')
				return c - 'a' + 10;
			return c - '0';
		}

		template<Type::ASCII T>
		constexpr bool isDigitInBase(T const& c, usize const& base) {
			ssize const v = toDigit(c);
			return 0 <= v && v < base;
		}

		template<Type::ASCII T>
		constexpr bool isSign(T const& c) {
			return c == '-' || c == '+';
		}

		template<Type::ASCII T>
		constexpr int8 getSignAndConsume(T*& c) {
			switch (c[0]) {
				case '-':	++c; return -1;
				case '+':	++c;
				default:	return +1;
			}
		}

		template<Type::Integer T>
		constexpr T shiftAndAppend(T& val, T const& base, T const& digit) {
			val *= base;
			val += digit;
		}

		template<Type::Integer I, Type::ASCII T>
		constexpr I toInteger(T const* str, usize const& size, usize const& base) {
			I res = 0;
			for (usize i = 0; i < size; ++i)
				shiftAndAppend(res, base, toDigit(str[i]));
			return res;
		}

		template<Type::ASCII T>
		constexpr usize getBaseAndConsume(T const*& c, usize const& base) {
			if (c[0] == '0') {
				++c;
				switch (c[0]) {
					case 'y':	++c; return base ? base : 32;
					case 'x':	++c; return base ? base : 16;
					case 'q':	++c; return base ? base : 4;
					case 'b':	++c; return base ? base : 2;
					case 'd':	++c; return base ? base : 10;
					case 'o':	++c;
					default:	return base ? base : 8;
				}
			}
			return 10;
		}

		template<Type::ASCII T>
		constexpr bool isInBase(T const* str, usize const& size, usize const& base) {
			for (usize i = 0; i < size; ++i)
				if(!isDigitInBase(str[i], base))
					return false;
			return true;
		}
	}
}

template<Type::Integer I, Type::ASCII T>
constexpr bool atoi(T const* const& str, usize size, I& out, usize base = 0) {
	// Copy string pointer
	T const* s = str;
	// If string is size 1, try and convert digit
	if (size == 1) {
		if (Impl::A2I::isDigitInBase(str[0], base))
			return Impl::A2I::toDigit(str[0]);
		else return false;
	}
	// If size is 2, first character is a sign, and is in base, convert number to digit
	if (
		size == 2
	&&	Impl::A2I::isSign(str[0])
	&&	Impl::A2I::isDigitInBase(str[1], base)
	) return Impl::A2I::getSignAndConsume(s, base) * Impl::A2I::toDigit(s[0]);
	// Try and get sign of number
	ssize sign = Impl::A2I::getSignAndConsume(s, base);
	if (size < s-str) return false;
	// Try and get base of number
	base = Impl::A2I::getBaseAndConsume(s, base);
	if (size < s-str) return false;
	// Remove difference from size
	size -= s-str;
	// Check if number is in its intended base
	if (!Impl::A2I::isInBase(s, size, base))
		return false;
	// Convert number to int and return
	out = sign * Impl::A2I::toInteger<I>(s, size, base);
	return true;
}

template<Type::Integer I, Type::ASCII T, usize S>
constexpr bool atoi(Decay::AsType<const T[S]> const& str, I& out) {
	static_assert(S, "String cannot be empty!");
	return atoi<I>(str, S - 1, out);
}

template<Type::Real F, Type::ASCII T>
constexpr bool atof(T const* const& str, usize size, F& out) {
	// If character is appended to the end, exclude it
	if (
		toLower(str[size-1]) == 'f'
	||	toLower(str[size-1]) == 'd'
	) --size;
	// Find separator character
	usize sep = 0;
	while (sep < size)
		if (str[sep++] == '.')
			break;
	// If no separator was found, convert number and return
	ssize ival = 0;
	if (sep == size) {
		if (!atoi<ssize>(str, size, ival))
			return false;
		out = ival;
		return true;
	}
	// Create new string without separator
	T* ns = new T[size-1];
	MX::memcpy(ns, str, sep-1);
	MX::memcpy(ns+sep-1, str+sep, size-sep);
	// Try and convert resulting integer string
	if (!atoi<usize>(ns, size-1, ival))
		return false;
	delete[] ns;
	// Convert integer to string by "reverse scientific notation" and return
	out = ival * pow<F>(10, -ssize(size-sep));
	return true;
}

template<Type::Real F, Type::ASCII T, usize S>
constexpr bool atof(Decay::AsType<const T[S]> const& str, F& out) {
	static_assert(S, "String cannot be empty!");
	return atof<F>(str, S - 1, out);
}

// Based off of https://stackoverflow.com/a/3987783
template<Type::Integer I, Type::ASCII T>
constexpr ssize itoa(I val, T* const& buf, usize const& bufSize, I const& base = 10){
	// If empty buffer, or buffer is too small for a non-decimal base
	if ((!bufSize) || (bufSize < 4 && base != 10))
		return -1;
	// Clear buffer
	memset(buf, 0, bufSize);
	// Get stating points
	usize
		offset = 0,
		i = bufSize-2
	;
	// If value is 0, set buffer to zero and return
	if (!val) {
		buf[0] = '0';
		return 0;
	}
	// If value is negative, append negative sign and invert value
	if (val < 0) {
		buf[offset] = '-';
		val = -val;
		++offset;
	}
	// If not decimal, append base identifier accoordingly
	if (base != 10) {
		buf[offset] = '0';
		++offset;
		switch (base) {
			case 2:		buf[offset] = 'b'; ++offset; break;
			case 4:		buf[offset] = 'q'; ++offset; break;
			case 16:	buf[offset] = 'x'; ++offset; break;
			case 32:	buf[offset] = 'y'; ++offset; break;
			default:
			case 8:		break;
		}
	}
	// Calculate number value
	for(; val && (i-offset); --i, val /= base)
		buf[i] = "0123456789abcdef"[val % base];
	// Move stuff around to beginning of buffer
	MX::memcpy(buf+offset, buf+offset+i, bufSize-i);
	// Return full size of number string
	return offset+i+1;
}

template<Type::Real F, Type::ASCII T>
constexpr ssize ftoa(F val, T* const& buf, usize const& bufSize, usize const& precision = sizeof(F)*2) {
	// Get amount of zeroes to add to number
	usize zeroes = pow<F>(10, precision);
	// Get whole part of number
	ssize whole = val;
	// Get fractional part
	usize frac = (val * zeroes) - (whole * zeroes);
	ssize lhs = 0, rhs = 0;
	// Fill in whole part of number to string, return if error
	if (lhs = itoa<ssize>(whole, buf, bufSize)) return -1;
	// Check if buffer is not full, else append comma and re-check
	if (lhs >= bufSize) return lhs;
	buf[lhs++] = '.';
	if (lhs >= bufSize) return lhs;
	// Fill in fractional part, returning if error
	if (rhs = itoa<ssize>(frac, buf+lhs, bufSize-lhs)) return -1;
	// Return full size of number string
	return lhs+rhs+1;
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_ATOI_H

#ifndef CTL_ALGORITHM_ATOI_H
#define CTL_ALGORITHM_ATOI_H

#include "transform.hpp"
#include "../cmath.hpp"

// atoi implementation based off of https://stackoverflow.com/a/59537554
namespace _AtoiImpl {
	template<ASCIIType T>
	constexpr intmax toDigit(T c) {
		c = toLower(c);
		if (c >= 'a')
			return c - 'a' + 10;
		return c - '0';
	}

	template<ASCIIType T>
	constexpr intmax isDigitInBase(T const& c, uintmax const& base) {
		uint8 const v = toDigit(c);
		return 0 <= digit && digit < base;
	}

	template<ASCIIType T>
	constexpr bool isSign(T const& c) {
		return c == '-' || c == '+';
	}

	template<ASCIIType T>
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

	template<Type::Integral I, ASCIIType T>
	constexpr I toInteger(T const* str, uintmax const& size, uintmax const& base) {
		I res = 0;
		for (uintmax i = 0; i < size; ++i)
			shiftAndAppend(res, base, toDigit(str[i]));
		return value;
	}

	template<ASCIIType T>
	constexpr uintmax getBaseAndConsume(T const*& c) {
		if (c[0] == '0') {
			++c;
			switch (c[0]) {
				case 'y':	++c; return 32;
				case 'x':	++c; return 16;
				case 'q':	++c; return 4;
				case 'b':	++c; return 2;
				case 'd':	++c; return 10;
				case 'o':	++c;
				default:	return 8;
			}
		}
		return 10;
	}

	template<ASCIIType T>
	constexpr bool isInBase(T const* str, uintmax const& size, uintmax const& base) {
		for (uintmax i = 0; i < size; ++i)
			if(!isDigitInBase(str[i], base))
				return false;
		return true;
	}
}

template<Type::Integer I, ASCIIType T>
constexpr bool atoi(T const* const& str, uintmax size, I& out) {
	// Copy string pointer
	T const* s = str;
	// If string is size 1, try and convert digit
	if (size == 1) {
		if (isDigitInBase(str[0], 10)) return toDigit(str[0]);
		else return false;
	}
	// If size is 2, first character is a sign, and is in base 10, convert number to digit
	if (
		size == 2
	&&	_AtoiImpl::isSign(str[0])
	&&	isDigitInBase(str[1], 10)
	) return getSignAndConsume(s) * toDigit(s[0]);
	// Try and get sign of number
	intmax sign = _AtoiImpl::getSignAndConsume(s);
	if (size < s-str) return false;
	// Try and get base of number
	intmax base = _AtoiImpl::getBaseAndConsume(s);
	if (size < s-str) return false;
	// Remove difference from size
	size -= s-str;
	// Check if number is in its intended base
	if (!_AtoiImpl::isInBase(s, size, base))
		return false;
	// Convert number to int and return
	out = sign * _AtoiImpl::toInteger<I>(s, size, base);
	return true;
}

template<Type::Integer I, ASCIIType T, uintmax S>
constexpr bool atoi(const T(const& str)[S], I& out) {
	static_assert(S, "String cannot be empty!");
	return atoi<I>(str, S - 1, out);
}

template<Type::Real F, ASCIIType T>
constexpr bool atof(T const* const& str, uintmax size, F& out) {
	// If character is appended to the end, exclude it
	if (
		toLower(str[size-1]) == 'f'
	||	toLower(str[size-1]) == 'd'
	) --size;
	// Find separator character
	uintmax sep = 0;
	while (sep < size)
		if (str[sep++] == '.')
			break;
	// If no separator was found, convert number and return
	intmax ival = 0;
	if (sep == size) {
		if (!atoi<intmax>(str, size, ival))
			return false;
		out = ival;
		return true;
	}
	// Create new string without separator
	T* ns = new T[size-1];
	memcpy(ns, str, sep-1);
	memcpy(ns+sep-1, str+sep, size-sep);
	// Try and convert resulting integer string
	if (!atoi<uintmax>(ns, size-1, ival))
		return false;
	delete[] ns;
	// Convert integer to string by "reverse scientific notation" and return
	out = ival * pow<F>(10, -intmax(size-sep));
	return true;
}

template<Type::Real F, ASCIIType T, uintmax S>
constexpr bool atof(const T(const& str)[S], F& out) {
	static_assert(S, "String cannot be empty!");
	return atof<F>(str, S - 1, out);
}

// Based off of https://stackoverflow.com/a/3987783
template<Type::Integer I, ASCIIType T>
constexpr intmax itoa(I val, T* const& buf, uintmax const& bufSize, I const& base = 10){
	// If empty buffer, or buffer is too small for a non-decimal base
	if ((!bufSize) || (bufSize < 4 && base != 10))
		return -1;
	// Clear buffer
	memset(buf, 0, bufSize);
	// Get stating points
	uintmax
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
	memcpy(buf+offset, buf+offset+i, bufSize-i);
	// Return full size of number string
	return offset+i+1;
}

template<Type::Real F, ASCIIType T>
constexpr intmax ftoa(F val, T* const& buf, uintmax const& bufSize, uintmax const& precision = sizeof(F)*4) {
	// Get amount of zeroes to add to number
	uintmax zeroes = pow<F>(10, precision);
	// Get whole part of number
	intmax whole = val;
	// Get fractional part
	uintmax frac = (val * zeroes) - (whole * zeroes);
	intmax lhs = 0, rhs = 0;
	// Fill in whole part of number to string, return if error
	if (lhs = itoa<intmax>(whole, buf, bufSize)) return -1;
	// Check if buffer is not full, else append comma and re-check
	if (lhs >= bufSize) return lhs;
	buf[lhs++] = '.';
	if (lhs >= bufSize) return lhs;
	// Fill in fractional part, returning if error
	if (rhs = itoa<intmax>(frac, buf+lhs, bufSize-lhs)) return -1;
	// Return full size of number string
	return lhs+rhs+1;
}

#endif // CTL_ALGORITHM_ATOI_H

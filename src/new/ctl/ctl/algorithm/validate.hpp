#ifndef CTL_ALGORITHM_VALIDATE_H
#define CTL_ALGORITHM_VALIDATE_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "transform.hpp"

CTL_NAMESPACE_BEGIN

template<Type::ASCII TChar>
constexpr bool isHexChar(TChar const& c) noexcept {
	if (c >= '0' && c <= '9') return true;
	if (c >= 'A' && c <= 'F') return true;
	if (c >= 'a' && c <= 'f') return true;
	return false;
}

template <Type::ASCII TChar>
constexpr bool isNullOrSpaceChar(TChar const& c) noexcept {
	return (
		c == '\0'
	||	c == ' '
	||	c == '\t'
//	||	c == '\n'
	);
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_VALIDATE_H

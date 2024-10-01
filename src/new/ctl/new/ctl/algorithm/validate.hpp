#ifndef CTL_ALGORITHM_VALIDATE_H
#define CTL_ALGORITHM_VALIDATE_H

#include "../namespace.hpp"
#include "../typetraits/traits.hpp"
#include "transform.hpp"

CTL_NAMESPACE_BEGIN

template<Type::Character TChar>
constexpr bool isHexChar(TChar const& c) {
	if (c >= '0' && c <= '9') return true;
	if (c >= 'A' && c <= 'F') return true;
	if (c >= 'a' && c <= 'f') return true;
	return false;
}

CTL_NAMESPACE_END

#endif // CTL_ALGORITHM_VALIDATE_H

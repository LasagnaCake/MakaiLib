#ifndef CTL_CMATH_H
#define CTL_CMATH_H

#include "namespace.hpp"

CTL_NAMESPACE_BEGIN

template<Type::Real F>
constexpr F pow(F const& value, F const& power) {
	if constexpr (Type::Equal<F, float>)
		return __builtin_powf(value, power);
	else if constexpr (Type::Equal<F, double>)
		return __builtin_pow(value, power);
	else
		return __builtin_powd(value, power);
}

CTL_NAMESPACE_END

#endif // CTL_CMATH_H

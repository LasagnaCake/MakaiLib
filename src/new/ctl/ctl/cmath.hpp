#ifndef CTL_CMATH_H
#define CTL_CMATH_H

#include "namespace.hpp"
#include "typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Calculates a value raised to a given power.
/// @tparam F Floating point type.
/// @param value Value to raise.
/// @param power Power to raise by.
/// @return Value raised to the given power.
template<Type::Real F>
constexpr F pow(F const& value, F const& power) {
	if constexpr (Type::Equal<F, float>)
		return __builtin_powf(value, power);
	else if constexpr (Type::Equal<F, double>)
		return __builtin_pow(value, power);
	else
		return __builtin_powl(value, power);
}

CTL_NAMESPACE_END

#endif // CTL_CMATH_H

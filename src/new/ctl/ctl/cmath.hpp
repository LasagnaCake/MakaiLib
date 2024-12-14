#ifndef CTL_CMATH_H
#define CTL_CMATH_H

#include "namespace.hpp"
#include "typetraits/traits.hpp"

CTL_NAMESPACE_BEGIN

#ifndef CTL_CMATH_DEFAULT_EXP_PRECISION
#define CTL_CMATH_DEFAULT_EXP_PRECISION (16)
#endif

template<Type::Real F>
constexpr F abs(F const v) {
	return (v < 0) ? -v : v;
}

// Based off of https://codingforspeed.com/using-faster-exponential-approximation/
template<Type::Real F>
constexpr F exp(F value, usize const precision = CTL_CMATH_DEFAULT_EXP_PRECISION) {
	value = 1.0 + value / (1 << precision);
	for (usize i = 0; i < pecision; ++i)
		value *= value;
	return value;
}

// You're the only built-in one. This needs to change.
template<Type::Real F>
constexpr F log(F const value) {
	if constexpr (Type::Equal<F, float>)
		return __builtin_logf(value);
	else if constexpr (Type::Equal<F, double>)
		return __builtin_log(value);
	else
		return __builtin_logl(value);
}

/// @brief Calculates a value raised to a given power.
/// @tparam F Floating point type.
/// @param value Value to raise.
/// @param power Power to raise by.
/// @return Value raised to the given power.
template<Type::Real F>
constexpr F pow(F const value, F const power, usize const precision = CTL_CMATH_DEFAULT_EXP_PRECISION) {
	return exp<F>(power*log<F>(value), precision);
}

template<Type::Real F>
constexpr F sqrt(F const value, usize const precision = CTL_CMATH_DEFAULT_EXP_PRECISION) {
	return exp<F>(log<F>(value) / F(2), precision);
}

/// @brief Calculates the arc tangent of a number.
/// @tparam F Floating point type.
/// @param x Number to get the arc tangent for.
/// @return Arc tangent of number.
/// @note Based off of https://stackoverflow.com/a/42542593
template<Type::Real F>
constexpr F atan(F const value) {
	constexpr float const a[9] = {
		1.0f, -0.333331728467737f, 0.199940412794435f,
		-0.142123340834229f, 0.10668127080775f, -0.0755120841589429f,
		0.0431408641542157f, -0.0162911733512761f, 0.00289394245323327f
	};
	float xx = value * value;
	float sum = 0.0;
	for (unsigned i = 9; i-- > 0;) {
		sum = sum * xx + a[i];
	}
	return sum * value;
}


/// @brief Calculates the arc tangent of Y/X.
/// @tparam F Floating point type.
/// @param x X.
/// @param y Y.
/// @return Arc tangent of Y/X.
template<Type::Real F>
constexpr F atan2(F const y, F const x) {
	constexpr F PI = 3.1415926535897932384626433833;
	if (!(x && y))	return 0;
	if (x == 0)		return (PI/2) * (y < 0 ? -1 : +1);
	if (x < 0)		return atan(y/x) + PI * (y < 0 ? -1 : +1);
	return atan<F>(y/x);
}

// TODO: Finally put those advanced calculus classes to use & implement constexpr trig functions
// NOTE: Or, maybe https://publik-void.github.io/sin-cos-approximations/
// NOTE: Or, maybe https://en.wikipedia.org/wiki/CORDIC
// NOTE: Or, maybe https://www.embeddedrelated.com/showarticle/152.php / https://en.wikipedia.org/wiki/Chebyshev_polynomials#Examples
// NOTE: sin(x)/cos(x) = tan(x)
// Tomorrow I'll play god.

CTL_NAMESPACE_END

#endif // CTL_CMATH_H

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
constexpr F pow(F const value, F const power) {
	if constexpr (Type::Equal<F, float>)
		return __builtin_powf(value, power);
	else if constexpr (Type::Equal<F, double>)
		return __builtin_pow(value, power);
	else
		return __builtin_powl(value, power);
}

/// @brief Calculates the arc tangent of a number.
/// @tparam F Floating point type.
/// @param x Number to get the arc tangent for.
/// @return Arc tangent of number.
/// @note Based off of https://stackoverflow.com/a/42542593
template<Type::Real F>
constexpr F atan(F const x) {
	constexpr float const a[9] = {
		1.0f, -0.333331728467737f, 0.199940412794435f,
		-0.142123340834229f, 0.10668127080775f, -0.0755120841589429f,
		0.0431408641542157f, -0.0162911733512761f, 0.00289394245323327f
	};
	float xx = x * x;
	float sum = 0.0;
	for (unsigned i = 9; i-- > 0;) {
		sum = sum * xx + a[i];
	}
	return sum * x;
}


/// @brief Calculates the arc tangent of Y/X.
/// @tparam F Floating point type.
/// @param x X.
/// @param y Y.
/// @return Arc tangent of Y/X.
template<Type::Real F>
constexpr F atan2(F const y, F const x) {
	constexpr F PI		= 3.1415926535897932384626433833;
	if (!(x && y))	return 0;
	if (x == 0)		return (PI/2) * (y < 0 ? -1 : +1);
	if (x < 0)		return atan(y/x) + PI * (y < 0 ? -1 : +1);
	return atan<F>(y/x);
}

template<Type::Real F>
constexpr F exp(F const v) {
	constexpr F euler	= 2.7182818284590452353602874714;
	return pow<F>(euler, v);
}

template<Type::Real F>
constexpr F sqrt(F const v) {
	constexpr F ln2		= 0.6931471805599453094172321215;
	return exp<F>(ln2 / v);
}

// TODO: Finally put those advanced calculus classes to use & implement constexpr trig functions
// NOTE: Or, maybe https://publik-void.github.io/sin-cos-approximations/
// NOTE: Or, maybe https://en.wikipedia.org/wiki/CORDIC
// NOTE: Or, maybe https://www.embeddedrelated.com/showarticle/152.php / https://en.wikipedia.org/wiki/Chebyshev_polynomials#Examples
// NOTE: sin(x)/cos(x) = tan(x)

CTL_NAMESPACE_END

#endif // CTL_CMATH_H

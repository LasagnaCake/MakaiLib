#ifndef CTL_MATH_H
#define CTL_MATH_H

template<Type::Real F>
constexpr F pow(F const& value, F const& power) {
	if constexpr (Type::Equal<F, float>)
		return __builtin_powf(val, power);
	else if constexpr (Type::Equal<F, double>)
		return __builtin_pow(val, power);
	else
		return __builtin_powd(val, power);
}

#endif // CTL_MATH_H

#ifndef CTL_MATH_CORE_H
#define CTL_MATH_CORE_H

#include <cmath>
#include "../typetraits/traits.hpp"
#include "../namespace.hpp"
#include "../typeinfo.hpp"
#include "../ctypes.hpp"
#include "../container/string.hpp"
#include "../container/pair.hpp"
#include "../container/function.hpp"

CTL_NAMESPACE_BEGIN

namespace Math {
	/**
	* An 'Operatable' type is defined as:
	*	a)	A 'Mutable' type that can be operated on, with basic arithmetic
			expressions (+, -, *, /), with another value of the same type.
	*/
	template <typename T>
	concept Operatable = Type::Mutable<T> && Type::Arithmetic<T, T>;

	/// Math Constants.
	constexpr const double	sqrt2		= 1.4142135623730950488016887242;
	constexpr const double	hsqrt2		= sqrt2 / 2.0;
	constexpr const double	qsqrt2		= sqrt2 / 4.0;
	constexpr const double	sqrt3		= 1.7320508075688772935274463415;
	constexpr const double	hsqrt3		= sqrt2 / 2.0;
	constexpr const double	qsqrt3		= sqrt2 / 4.0;
	constexpr const double	ln2			= 0.6931471805599453094172321215;
	constexpr const double	pi			= 3.1415926535897932384626433833;
	constexpr const double	hpi			= pi / 2.0;
	constexpr const double	qpi			= pi / 4.0;
	constexpr const double	tau			= 6.2831853071795864769252867666;
	constexpr const double	euler		= 2.7182818284590452353602874714;
	constexpr const double	degrad		= 180.0 / pi;
	constexpr const double	maribel		= euler - (11.0 - 6.0 * sqrt2) / 5.0;
	constexpr const double	phi			= 1.6180339887498948482045868343;
	constexpr const double	tauphi		= tau / phi;

	template<Operatable T = float>
	constexpr T sign(T const& val)
	requires (
		Type::Comparable::All<T, T>
	&&	Type::Constructible<T, float>
	) {
		return (val < 0 ? -1 : (val > 0 ? +1 : 0));
	}

	template<Operatable T = float>
	constexpr T min(T const& a, T const& b)
	requires Type::Comparable::Lesser<T, T> {
		return ((a < b) ? a : b);
	}

	template<Operatable T = float>
	constexpr T max(T const& a, T const& b)
	requires Type::Comparable::Greater<T, T> {
		return ((a > b) ? a : b);
	}

	template<Operatable T = float>
	constexpr T clamp(T const& a, T const& min, T const& max)
	requires Type::Comparable::All<T, T> {
		return ((a < min) ? min : ((a > max) ? max : a));
	}

	template<Operatable T = float>
	constexpr T radians(T const& deg) requires Type::Arithmetic<T, float> {
		return (T)(deg / degrad);
	}

	template<Operatable T = float>
	constexpr T degrees(T const& rad) requires Type::Arithmetic<T, float> {
		return (T)(rad * degrad);
	}

	template<Operatable T = float>
	constexpr T lerp(T const& from, T const& to, T const& by) {
		return (T)(from + by * (to - from));
	}

	template<Type::Number T = float>
	constexpr T angleLerp(T const& from, T const& to, T const& by) {
		T dist	= fmod(to - from, tau);
		dist	= fmod(2.0 * dist, tau) - dist;
		return (T)(from + dist * by);
	}

	template<Type::Number T = float>
	constexpr T step(T const& val, T const& step) {
		if (step != 0)
			return (T)(floor(val / step + 0.5) * step);
		return val;
	}

	template<Type::Number T = float>
	constexpr T floor(T const& val) {
		return (T)(::floor(val));
	}

	template<Type::Number T = float>
	constexpr T floor(T const& val, int const& decimals) {
		// Get rounding factor
		T zeros = pow<floatmax>(10, decimals);
		// Floor it
		return (T)(::floor(val * zeros) / zeros);
	}

	template<Type::Number T = float>
	constexpr T ceil(T const& val) {
		return (T)(::ceil(val));
	}

	template<Type::Number T = float>
	constexpr T ceil(T const& val, int const& decimals) {
		// Get rounding factor
		T zeros = pow<floatmax>(10, decimals);
		// Ceil it
		return (T)(::ceil(val * zeros) / zeros);
	}

	template<Type::Number T = float>
	constexpr T round(T const& val) {
		// Add 1/2 & floor it
		return (T)(::floor(val + 0.5));
	}

	template<Type::Number T = float>
	constexpr T round(T const& val, int const& decimals) {
		// Get rounding factor
		T zeros = pow<floatmax>(10, decimals);
		// Add 1/2 & floor it
		return (T)(::floor((val + 0.5) * zeros) / zeros);
	}

	template<Type::Number T = float>
	constexpr T fmult(T const& val, T const& mult) {
		return ::floor(val / (double)mult) * mult;
	}

	template<Type::Number T = float>
	constexpr T cmult(T const& val, T const& mult) {
		return ::ceil(val / (double)mult) * mult;
	}

	template<Type::Number T = float>
	constexpr T mod(T const& a, T const& b) {
		return fmod(a, b);
	}

	template<Type::Number T = float>
	constexpr T ramp(T const& val) {
		return val - round(val);
	}

	template<Type::Number T = float>
	constexpr T fract(T const& val) {
		return val - ::floor(val);
	}

	template<Type::Integer T = float>
	constexpr T wrapi(T const& val, T const& min, T const& max) {
		return ((val - min) % (max - min)) + min;
	}

	template<Type::Number T = float>
	constexpr T wrap(T const& val, T const& min, T const& max) {
		return mod(val - min, max - min) + min;
	}

	template<Type::Number T = float>
	constexpr T nroot(T const& val, T const& root) {
		return exp(log(root) / val);
	}

	template<Type::Number T = float>
	constexpr T sqrt(T const& val) {
		return exp(ln2 / val);
	}

	template<Type::Number T = float>
	constexpr T nrtn(T const& val) {
		return nroot(val, val);
	}

	template<Type::Number T = float> constexpr T absin(T const& val) {return abs(sin(val));}
	template<Type::Number T = float> constexpr T abcos(T const& val) {return abs(cos(val));}
	template<Type::Number T = float> constexpr T abtan(T const& val) {return abs(tan(val));}

	/// Reflects a given angle A in relation to a surface along angle S.
	constexpr float reflect(float const& a, float const& s) {
		return (2.0 * s) - a;
	}

	template<Operatable T>
	constexpr inline Function<T(T const&)> getInterpolationFunction(T const& from, T const& to) {
		return [=](T const& by) -> T {return (T)lerp(from, to, by);};
	}
	
	template<Type::Integer T = int>
	constexpr inline usize uipow2(T const& val, usize const& power) {
		return val << power;
	}

	template<Type::Integer T = int>
	constexpr inline bool bit(T const& val, usize const& bit) {
		return (val & (0x1 << bit));
	}

	template <Type::Number T>
	constexpr Pair<int, T> frexp(T const& val) {
		int pow2 = 0;
		T rem = std::frexp((float)val, &pow2);
		return Pair<int, T>{pow2, rem};
	}

	template <Type::Number T>
	constexpr T ldexp(T const& val, int const& exp) {
		return (T)::ldexp((double)val, exp);
	}

	template <Type::Integer T>
	constexpr T factorial(T const& val) {
		if (val < 2) return val;
		else return val * factorial(val-1);
	}
}

CTL_NAMESPACE_END

#ifndef SQRT2
#define SQRT2 (::CTL::Math::sqrt2)
#endif // SQRT2

#ifndef HSQRT2
#define HSQRT2 (::CTL::Math::hsqrt2)
#endif // HSQRT2

#ifndef LN2
#define LN2 (::CTL::Math::ln2)
#endif // LN2

#ifndef PI
#define PI (::CTL::Math::pi)
#endif // PI

#ifndef HPI
#define HPI (::CTL::Math::hpi)
#endif // HPI

#ifndef QPI
#define QPI (::CTL::Math::qpi)
#endif // HPI

#ifndef TAU
#define TAU (::CTL::Math::tau)
#endif // TAU

#ifndef EULER
#define EULER (::CTL::Math::euler)
#endif // EULER

#ifndef PHI
#define PHI (::CTL::Math::phi)
#endif // PHI

#ifndef DEGRAD
#define DEGRAD (::CTL::Math::degrad)
#endif // DEGRAD

#ifndef MARIBEL
#define MARIBEL (::CTL::Math::maribel)
#endif // MARIBEL

#ifndef TAUPHI
#define TAUPHI (::CTL::Math::tauphi)
#endif // TAUPHI

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
constexpr float		operator ""f(unsigned long long v)	{return v;	}
constexpr float		operator ""F(unsigned long long v)	{return v;	}
constexpr double	operator ""d(unsigned long long v)	{return v;	}
constexpr double	operator ""D(unsigned long long v)	{return v;	}

CTL_NAMESPACE_BEGIN

// "Degrees to Radians" literal conversion
constexpr float			operator ""degf(long double d)			{return CTL::Math::radians<float>(d);		}
constexpr float			operator ""degf(unsigned long long d)	{return CTL::Math::radians<float>(d);		}
constexpr double		operator ""degd(long double d)			{return CTL::Math::radians<double>(d);		}
constexpr double		operator ""degd(unsigned long long d)	{return CTL::Math::radians<double>(d);		}
constexpr long double	operator ""degld(long double d)			{return CTL::Math::radians<long double>(d);	}
constexpr long double	operator ""degld(unsigned long long d)	{return CTL::Math::radians<long double>(d);	}
constexpr float			operator ""deg(long double d)			{return CTL::Math::radians<float>(d);		}
constexpr float			operator ""deg(unsigned long long d)	{return CTL::Math::radians<float>(d);		}
// "Radians to Degrees" literal conversion
#ifdef CTL_INCLUDE_RADIAN_CONVERSION_LITERALS
constexpr float			operator ""radf(long double r)			{return CTL::Math::degrees<float>(r);		}
constexpr float			operator ""radf(unsigned long long r)	{return CTL::Math::degrees<float>(r);		}
constexpr double		operator ""radd(long double r)			{return CTL::Math::degrees<double>(r);		}
constexpr double		operator ""radd(unsigned long long r)	{return CTL::Math::degrees<double>(r);		}
constexpr long double	operator ""radld(long double r)			{return CTL::Math::degrees<long double>(r);	}
constexpr long double	operator ""radld(unsigned long long r)	{return CTL::Math::degrees<long double>(r);	}
constexpr float			operator ""rad(long double r)			{return CTL::Math::degrees<float>(r);		}
constexpr float			operator ""rad(unsigned long long r)	{return CTL::Math::degrees<float>(r);		}
#endif // CTL_INCLUDE_RADIAN_CONVERSION_LITERALS

CTL_NAMESPACE_END

#pragma GCC diagnostic pop

#endif // CTL_MATH_CORE_H

#ifndef ALGEBRA_MATH_MATHEMATICS_H
#define ALGEBRA_MATH_MATHEMATICS_H

#include <string>
#include <cmath>
#include <random>
#include <limits>
#include "conceptual.hpp"
#include "helper.hpp"

#define ANYTYPE			template<Operatable T = float> T
#define ANYTYPE_I		template<Operatable T = float> inline T
#define CONST_ANYTYPE	template<Operatable T = float> constexpr T
#define CONST_ANYTYPE_I	template<Operatable T = float> constexpr inline T

namespace Math {
	namespace Max {
		constexpr auto CHAR_V		= std::numeric_limits<signed char>::max();
		constexpr auto UCHAR_V		= std::numeric_limits<unsigned char>::max();
		constexpr auto BYTE_V		= std::numeric_limits<signed char>::max();
		constexpr auto UBYTE_V		= std::numeric_limits<unsigned char>::max();
		constexpr auto SHORT_V		= std::numeric_limits<short>::max();
		constexpr auto USHORT_V		= std::numeric_limits<unsigned short>::max();
		constexpr auto INT_V		= std::numeric_limits<int>::max();
		constexpr auto UINT_V		= std::numeric_limits<unsigned int>::max();
		constexpr auto LONG_V		= std::numeric_limits<long>::max();
		constexpr auto ULONG_V		= std::numeric_limits<unsigned long>::max();
		constexpr auto LLONG_V		= std::numeric_limits<long long>::max();
		constexpr auto ULLONG_V		= std::numeric_limits<unsigned long long>::max();
		constexpr auto SIZET_V		= std::numeric_limits<size_t>::max();
		constexpr auto FLOAT_V		= std::numeric_limits<float>::max();
		constexpr auto DOUBLE_V		= std::numeric_limits<double>::max();
		constexpr auto LDOUBLE_V	= std::numeric_limits<long double>::max();

		constexpr auto INT8_V		= BYTE_V;
		constexpr auto UINT8_V		= UBYTE_V;
		constexpr auto INT16_V		= SHORT_V;
		constexpr auto UINT16_V		= USHORT_V;
		constexpr auto INT32_V		= LONG_V;
		constexpr auto UINT32_V		= ULONG_V;
		constexpr auto INT64_V		= LLONG_V;
		constexpr auto UINT64_V		= ULLONG_V;
		constexpr auto FLOAT32_V	= FLOAT_V;
		constexpr auto FLOAT64_V	= DOUBLE_V;
		constexpr auto FLOAT128_V	= LDOUBLE_V;

		template<Type::Number T>
		constexpr T ofType() {return std::numeric_limits<T>::max();}
	}

	namespace Min {
		constexpr auto BYTE_V		= std::numeric_limits<signed char>::lowest();
		constexpr auto UBYTE_V		= std::numeric_limits<unsigned char>::lowest();
		constexpr auto SHORT_V		= std::numeric_limits<short>::lowest();
		constexpr auto USHORT_V		= std::numeric_limits<unsigned short>::lowest();
		constexpr auto INT_V		= std::numeric_limits<int>::lowest();
		constexpr auto UINT_V		= std::numeric_limits<unsigned int>::lowest();
		constexpr auto LONG_V		= std::numeric_limits<long>::lowest();
		constexpr auto ULONG_V		= std::numeric_limits<unsigned long>::lowest();
		constexpr auto LLONG_V		= std::numeric_limits<long long>::lowest();
		constexpr auto ULLONG_V		= std::numeric_limits<unsigned long long>::lowest();
		constexpr auto SIZET_V		= std::numeric_limits<size_t>::lowest();
		constexpr auto FLOAT_V		= std::numeric_limits<float>::lowest();
		constexpr auto DOUBLE_V		= std::numeric_limits<double>::lowest();
		constexpr auto LDOUBLE_V	= std::numeric_limits<long double>::lowest();

		constexpr auto INT8_V		= BYTE_V;
		constexpr auto UINT8_V		= UBYTE_V;
		constexpr auto INT16_V		= SHORT_V;
		constexpr auto UINT16_V		= USHORT_V;
		constexpr auto INT32_V		= LONG_V;
		constexpr auto UINT32_V		= ULONG_V;
		constexpr auto INT64_V		= LLONG_V;
		constexpr auto UINT64_V		= ULLONG_V;
		constexpr auto FLOAT32_V	= FLOAT_V;
		constexpr auto FLOAT64_V	= DOUBLE_V;
		constexpr auto FLOAT128_V	= LDOUBLE_V;

		template<Type::Number T>
		constexpr T ofType() {return std::numeric_limits<T>::lowest();}
	}

	namespace Interval {
		constexpr auto CHAR_V		= 1;
		constexpr auto UCHAR_V		= 1;
		constexpr auto BYTE_V		= 1;
		constexpr auto UBYTE_V		= 1;
		constexpr auto SHORT_V		= 1;
		constexpr auto USHORT_V		= 1;
		constexpr auto INT_V		= 1;
		constexpr auto UINT_V		= 1;
		constexpr auto LONG_V		= 1;
		constexpr auto ULONG_V		= 1;
		constexpr auto LLONG_V		= 1;
		constexpr auto ULLONG_V		= 1;
		constexpr auto SIZET_V		= 1;
		constexpr auto FLOAT_V		= std::numeric_limits<float>::min();
		constexpr auto DOUBLE_V		= std::numeric_limits<double>::min();
		constexpr auto LDOUBLE_V	= std::numeric_limits<long double>::min();

		constexpr auto INT8_V		= BYTE_V;
		constexpr auto UINT8_V		= UBYTE_V;
		constexpr auto INT16_V		= SHORT_V;
		constexpr auto UINT16_V		= USHORT_V;
		constexpr auto INT32_V		= LONG_V;
		constexpr auto UINT32_V		= ULONG_V;
		constexpr auto INT64_V		= LLONG_V;
		constexpr auto UINT64_V		= ULLONG_V;
		constexpr auto FLOAT32_V	= FLOAT_V;
		constexpr auto FLOAT64_V	= DOUBLE_V;
		constexpr auto FLOAT128_V	= LDOUBLE_V;

		template<Type::Number T>
		constexpr T ofType() {return std::numeric_limits<T>::min();}
	}

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
	constexpr const double	infinity	= std::numeric_limits<double>::infinity();
	constexpr const float	infinityF	= std::numeric_limits<float>::infinity();
	constexpr const double	epsilon		= std::numeric_limits<double>::epsilon();
	constexpr const float	epsilonF	= std::numeric_limits<float>::epsilon();

	#ifndef SQRT2
	#define SQRT2 Math::sqrt2
	#endif // SQRT2

	#ifndef HSQRT2
	#define HSQRT2 Math::hsqrt2
	#endif // HSQRT2

	#ifndef LN2
	#define LN2 Math::ln2
	#endif // LN2

	#ifndef PI
	#define PI Math::pi
	#endif // PI

	#ifndef HPI
	#define HPI Math::hpi
	#endif // HPI

	#ifndef QPI
	#define QPI Math::qpi
	#endif // HPI

	#ifndef TAU
	#define TAU Math::tau
	#endif // TAU

	#ifndef EULER
	#define EULER Math::euler
	#endif // EULER

	#ifndef PHI
	#define PHI Math::phi
	#endif // PHI

	#ifndef DEGRAD
	#define DEGRAD Math::degrad
	#endif // DEGRAD

	#ifndef MARIBEL
	#define MARIBEL Math::maribel
	#endif // MARIBEL

	#ifndef TAUPHI
	#define TAUPHI Math::tauphi
	#endif // TAUPHI

	CONST_ANYTYPE_I sign(T const& val) requires Type::Comparable::All<T, T> {
		return (val < 0 ? -1 : (val > 0 ? +1 : 0));
	}

	CONST_ANYTYPE_I min(T const& a, T const& b) requires Type::Comparable::Lesser<T, T> {
		return ((a < b) ? a : b);
	}

	CONST_ANYTYPE_I max(T const& a, T const& b) requires Type::Comparable::Greater<T, T> {
		return ((a > b) ? a : b);
	}

	CONST_ANYTYPE_I clamp(T const& a, T const& min, T const& max) requires Type::Comparable::All<T, T> {
		return ((a < min) ? min : ((a > max) ? max : a));
	}

	CONST_ANYTYPE_I radians(T const& deg) requires Type::Arithmetic<T, float> {
		return (T)(deg / degrad);
	}

	CONST_ANYTYPE_I degrees(T const& rad) requires Type::Arithmetic<T, float> {
		return (T)(rad * degrad);
	}

	CONST_ANYTYPE_I linearToRadian(T const& val) requires Type::Arithmetic<T, float> {
		return (T)(val * tau);
	}

	CONST_ANYTYPE_I radianToLinear(T const& val) requires Type::Arithmetic<T, float> {
		return (T)(val / tau);
	}

	CONST_ANYTYPE_I linearToDegree(T const& val) requires Type::Arithmetic<T, float> {
		return (T)(val * 360.0);
	}

	CONST_ANYTYPE_I degreeToLinear(T const& val) requires Type::Arithmetic<T, float> {
		return (T)(val / 360.0);
	}

	CONST_ANYTYPE_I lerp(T const& from, T const& to, T const& by) {
		return (T)(from + by * (to - from));
	}

	CONST_ANYTYPE angleLerp(T const& from, T const& to, T const& by) requires Type::Number<T> {
		T dist	= fmod(to - from, tau);
		dist	= fmod(2.0 * dist, tau) - dist;
		return (T)(from + dist * by);
	}

	CONST_ANYTYPE_I step(T const& val, T const& step) requires Type::Number<T> {
		if (step != 0)
			return (T)(floor(val / step + 0.5) * step);
		return val;
	}

	CONST_ANYTYPE floor(T const& val) requires Type::Number<T> {
		return (T)(::floor(val));
	}

	CONST_ANYTYPE floor(T const& val, int const& decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Floor it
		return (T)(::floor(val * zeros) / zeros);
	}

	CONST_ANYTYPE ceil(T const& val) requires Type::Number<T> {
		return (T)(::ceil(val));
	}

	CONST_ANYTYPE ceil(T const& val, int const& decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Ceil it
		return (T)(::ceil(val * zeros) / zeros);
	}

	CONST_ANYTYPE_I round(T const& val) requires Type::Number<T> {
		// Add 1/2 & floor it
		return (T)(::floor(val + 0.5));
	}

	CONST_ANYTYPE round(T const& val, int const& decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Add 1/2 & floor it
		return (T)(::floor((val + 0.5) * zeros) / zeros);
	}

	CONST_ANYTYPE_I fmult(T const& val, T const& mult) requires Type::Number<T> {
		return ::floor(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I cmult(T const& val, T const& mult) requires Type::Number<T> {
		return ::ceil(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I mod(T const& a, T const& b) requires Type::Number<T> {
		return fmod(a, b);
	}

	CONST_ANYTYPE_I ramp(T const& val) requires Type::Number<T> {
		return val - round(val);
	}

	CONST_ANYTYPE_I fract(T const& val) requires Type::Number<T> {
		return val - ::floor(val);
	}

	CONST_ANYTYPE_I wmax(T const& val, T const& max) requires Type::Number<T> {
		T result = mod(val, max);
		if (result < 0) result += max;
		return result;
	}

	CONST_ANYTYPE_I wrap(T const& val, T const& min, T const& max) requires Type::Number<T> {
		T range = max - min;
		T result = mod(val - min, range);
		if (result < 0) result += range;
		return result + min;
	}

	CONST_ANYTYPE_I nroot(T const& val, T const& root) requires Type::Number<T> {
		return exp(log(root) / val);
	}

	CONST_ANYTYPE_I sqrt(T const& val) requires Type::Number<T> {
		return exp(ln2 / val);
	}

	CONST_ANYTYPE_I nrtn(T const& val) requires Type::Number<T> {
		return nroot(val, val);
	}

	CONST_ANYTYPE_I absin(T const& val) requires Type::Number<T> {return abs(sin(val));}
	CONST_ANYTYPE_I abcos(T const& val) requires Type::Number<T> {return abs(cos(val));}
	CONST_ANYTYPE_I abtan(T const& val) requires Type::Number<T> {return abs(tan(val));}

	inline size_t digitCount(size_t number) {
		return toString(number).size();
	}

	inline unsigned char digit(size_t const& val, unsigned char const& digit) {
		std::string vstr = toString(val);
		return vstr[(vstr.size()-1) - digit] - '0';
	}

	/**
	* Returns the polar radius of a point along the edges of a N-sided
	* polygon of "size" S (the shape's circumradius) at a given angle THETA,
	* rotated by angle A.
	*/

	/*
		Based off of the formula from https://www.desmos.com/calculator/hghrnwbcdh
		(Archive) https://web.archive.org/web/20240209160635/https://www.desmos.com/calculator/hghrnwbcdh
	*/
	/// Precise version. Better results, but more computationally intensive.
	constexpr float precisePolarPolyPoint(
		float const& theta,
		float const& sides,
		float const& size	= 1,
		float const& angle	= 0
	) {
		float const constant	= size * cos(pi / sides);
		float const shape		= (sides / 2.0) * theta + angle;
		return constant / cos((2.0 / sides) * asin(cos(shape)));
	}

	/// General version. Not as accurate, but still good for general use.
	constexpr float polarPolyPoint(
		float const& theta,
		float const& sides,
		float const& size	= 1,
		float const& angle	= 0
	) {
		float const constant	= (size * sqrt3) / (sides * sqrt(sides));
		float const shape		= (theta * sides + angle - hpi) / 2.0;
		return size - constant * abcos(shape);
	}

	/// Fast version. Least accurate, but computationally fast.
	constexpr float fastPolarPolyPoint(
		float const& theta,
		float const& sides,
		float const& size	= 1,
		float const& angle	= 0
	) {
		float const constant	= (size * sqrt2) / (sides * 2.0);
		float const shape		= (theta * sides + angle - hpi) / 2.0;
		return size - constant * abcos(shape);
	}

	/// Reflects a given angle A in relation to a surface along angle S.
	constexpr float reflect(float const& a, float const& s) {
		return (2.0 * s) - a;
	}

	CONST_ANYTYPE_I farenheitToCelcius(T const& temperature) {
		return (temperature - 32.0) * (5.0/9.0);
	}

	CONST_ANYTYPE_I celciusToFarenheit(T const& temperature) {
		return (temperature / (5.0/9.0)) + 32.0;
	}

	template<Operatable T>
	constexpr inline std::function<T(T const&)> getInterpolationFunction(T const& from, T const& to) {
		return [=](T const& by) -> T {return (T)lerp(from, to, by);};
	}

	inline float dbToVolume(float const& db) {
		return powf(10.0f, 0.5f * db);
	}

	constexpr inline float volumeToDb(float const& volume) {
		return 20.0f * log10f(volume);
	}

	template<Type::Integer T = int>
	constexpr inline size_t uipow2(T const& val, size_t const& power) {
		return val << power;
	}

	template<Type::Integer T = int>
	constexpr inline bool bit(T const& val, size_t const& bit) {
		return (val & (0x1 << bit));
	}

	template <Operatable T>
	constexpr std::pair<int, T> frexp(T const& val) requires Type::Number<T> {
		int pow2 = 0;
		T rem = std::frexp((float)val, &pow2);
		return std::pair<int, T>{pow2, rem};
	}

	CONST_ANYTYPE_I ldexp(T const& val, int const& exp) requires Type::Number<T> {
		return (T)::ldexp((double)val, exp);
	}

	CONST_ANYTYPE_I factorial(T const& val) requires Type::Integer<T> {
		if (val < 2) return val;
		else return val * factorial(val-1);
	}

	namespace Random {
		namespace {
			template<typename T>
			using IntDist = std::uniform_int_distribution<T>;

			template<typename T>
			using RealDist = std::uniform_real_distribution<T>;

			typedef std::mt19937_64 RandomEngine;
			// The random number generator engine used
			RandomEngine	engine{Time::sinceEpoch<Time::Millis>()};
			// Default distributions
			IntDist<size_t>		longDist(0, Math::Max::SIZET_V);
			RealDist<double>	realDist(0.0,1.0);
		}
		typedef RandomEngine Engine;

		template <Type::Number T>
		using Distribution = Meta::DualType<Type::Integer<T>, IntDist<T>, RealDist<T>>;

		/// Returns a random double between 0 and 1.
		inline double real() {
			return realDist(engine);
		}

		/// Returns a random double between the given values.
		template<Type::Real T = float>
		inline T real(T const& min, T const& max) {
			RealDist<T> dist(min, max);
			return dist(engine);
		}

		/// Returns a random integer between 0 and maximum size_t.
		inline size_t integer() {
			return longDist(engine);
		}

		/// Returns a random integer between the given values.
		template<Type::Integer T = int>
		inline T integer(T const& min, T const& max) {
			IntDist<T> dist(min, max);
			return dist(engine);
		}

		template<Type::Number T = int, class E>
		inline T custom(T const& min, T const& max, E& engine) {
			Distribution<T> dist(min, max);
			return dist(engine);
		}

		template<Type::Integer T>
		inline T number()							{return integer<T>();			}

		template<Type::Real T>
		inline T number() 							{return real<T>();				}

		template<Type::Integer T>
		inline T number(T const& min, T const& max) {return integer<T>(min, max);	}

		template<Type::Real T>
		inline T number(T const& min, T const& max) {return real<T>(min, max);		}

		/// Sets the random number generator's seed.
		inline void setSeed(size_t const& seed) {
			engine = RandomEngine{seed};
		}

		/// Gets a seed based on the current clock's time, and a random number.
		inline size_t getNewSeed() {
			return Time::sinceEpoch<Time::Millis>() ^ !(integer() << 0x2F);
		}
	}
}
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
// "Degrees to Radians" literal conversion
constexpr long double operator ""deg(long double d)	{return Math::radians<long double>(d);}
// "Radians to Degrees" literal conversion
//constexpr long double operator ""rad(long double r)	{return Math::degrees(r);}
#pragma GCC diagnostic pop

#define RNG	Math::Random

#undef CONST_ANYTYPE_I
#undef CONST_ANYTYPE
#undef ANYTYPE_I
#undef ANYTYPE

#endif // ALGEBRA_MATH_MATHEMATICS_H

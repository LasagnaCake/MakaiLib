#ifndef ALGEBRA_MATH_MATHEMATICS_H
#define ALGEBRA_MATH_MATHEMATICS_H

#include <string>
#include <cmath>
#include <random>
#include <limits>
#include "conceptual.hpp"
#include "helper.hpp"

#define ANYTYPE			template<Operatable T> T
#define ANYTYPE_I		template<Operatable T> inline T
#define CONST_ANYTYPE	template<Operatable T> constexpr T
#define CONST_ANYTYPE_I	template<Operatable T> constexpr inline T

#define $maxof(TYPE) (std::numeric_limits<TYPE>::max())

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
	constexpr const double	maribel		= euler - (sqrt2 - 1.0) * (sqrt2 - 0.2);
	constexpr const double	phi			= 1.6180339887498948482045868343;
	constexpr const double	tauphi		= tau / phi;
	constexpr const size_t	maxSizeT	= $maxof(size_t);
	constexpr const double	infinity	= std::numeric_limits<double>::infinity();
	constexpr const float	infinityF	= std::numeric_limits<float>::infinity();
	constexpr const double	epsilon		= 1.0 / infinity;
	constexpr const float	epsilonF	= 1.0 / infinityF;

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
	#endif // DEGRAD

	CONST_ANYTYPE_I sign(T val) requires Type::Comparable<T, T> {
		return (val < 0 ? -1 : (val > 0 ? +1 : 0));
	}

	CONST_ANYTYPE_I min(T a, T b) requires Type::Comparable<T, T> {
		return ((a < b) ? a : b);
	}

	CONST_ANYTYPE_I max(T a, T b) requires Type::Comparable<T, T> {
		return ((a > b) ? a : b);
	}

	CONST_ANYTYPE_I clamp(T a, T min, T max) requires Type::Comparable<T, T> {
		return ((a < min) ? min : ((a > max) ? max : a));
	}

	CONST_ANYTYPE_I radians(T deg) requires Type::Arithmetic<T, float> {
		return (T)(deg / degrad);
	}

	CONST_ANYTYPE_I degrees(T rad) requires Type::Arithmetic<T, float> {
		return (T)(rad * degrad);
	}

	CONST_ANYTYPE_I linearToRadian(T val) requires Type::Arithmetic<T, float> {
		return (T)(val * tau);
	}

	CONST_ANYTYPE_I radianToLinear(T val) requires Type::Arithmetic<T, float> {
		return (T)(val / tau);
	}

	CONST_ANYTYPE_I linearToDegree(T val) requires Type::Arithmetic<T, float> {
		return (T)(val * 360.0);
	}

	CONST_ANYTYPE_I degreeToLinear(T val) requires Type::Arithmetic<T, float> {
		return (T)(val / 360.0);
	}

	CONST_ANYTYPE_I lerp(T from, T to, T by) {
		return (T)(from + by * (to - from));
	}

	CONST_ANYTYPE angleLerp(T from, T to, T by) requires Type::Number<T> {
		T dist	= fmod(to - from, tau);
		dist	= fmod(2.0 * dist, tau) - dist;
		return (T)(from + dist * by);
	}

	CONST_ANYTYPE_I step(T val, T step) requires Type::Number<T> {
		if (step != 0)
			return (T)(floor(val / step + 0.5) * step);
		return val;
	}

	CONST_ANYTYPE floor(T val) requires Type::Number<T> {
		return (T)(::floor(val));
	}

	CONST_ANYTYPE floor(T val, int decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Floor it
		return (T)(::floor(val * zeros) / zeros);
	}

	CONST_ANYTYPE ceil(T val) requires Type::Number<T> {
		return (T)(::ceil(val));
	}

	CONST_ANYTYPE ceil(T val, int decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Ceil it
		return (T)(::ceil(val * zeros) / zeros);
	}

	CONST_ANYTYPE_I round(T val) requires Type::Number<T> {
		// Add 1/2 & floor it
		return (T)(::floor(val + 0.5));
	}

	CONST_ANYTYPE round(T val, int decimals) requires Type::Number<T> {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Add 1/2 & floor it
		return (T)(::floor((val + 0.5) * zeros) / zeros);
	}

	CONST_ANYTYPE_I fmult(T val, T mult) requires Type::Number<T> {
		return ::floor(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I cmult(T val, T mult) requires Type::Number<T> {
		return ::ceil(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I mod(T a, T b) requires Type::Number<T> {
		return b * ::floor(a/b);
	}

	CONST_ANYTYPE_I rmod(T a, T b) requires Type::Number<T> {
		return b * ::ceil(a/b);
	}

	CONST_ANYTYPE_I wmax(T val, T max) requires Type::Number<T> {
		return val - mod(val, max);
	}

	CONST_ANYTYPE_I wmin(T val, T min) requires Type::Number<T> {
		return val + rmod(val, min);
	}

	CONST_ANYTYPE_I wrap(T val, T min, T max) requires Type::Number<T> {
		return wmax(val + min, max) - min;
	}

	CONST_ANYTYPE_I nroot(T val, T root) requires Type::Number<T> {
		return exp(log(root) / val);
	}

	CONST_ANYTYPE_I sqrt(T val) requires Type::Number<T> {
		return exp(ln2 / val);
	}

	CONST_ANYTYPE_I nrtn(T val) requires Type::Number<T> {
		return nroot(val, val);
	}

	inline size_t digitCount(size_t number) {
		return std::to_string(number).size();
	}

	inline unsigned char digit(size_t val, unsigned char digit) {
		std::string vstr = std::to_string(val);
		return vstr[(vstr.size()-1) - digit] - '0';
	}

	/**
	* Returns the polar radius of a point along the edges of a N-sided
	* polygon of "size" S (the shape's circumradius) at a given angle THETA,
	* rotated by angle A.
	*/
	constexpr float polarPolyPoint(
		float theta,
		float sides,
		float size	= 1,
		float angle	= 0
	) {
		float constant	= (size * sqrt3) / (sides * sqrt(sides));
		float shape		= (theta * sides + angle - hpi) / 2.0;
		return size - constant * abs(cos(shape));
	}

	constexpr float fastPolarPolyPoint(
		float theta,
		float sides,
		float size	= 1,
		float angle	= 0
	) {
		float constant	= (size * sqrt2) / (sides * 2.0);
		float shape		= (theta * sides + angle - hpi) / 2.0;
		return size - constant * abs(cos(shape));
	}

	/// Reflects a given angle A in relation to a surface along angle S.
	constexpr float reflect(float a, float s) {
		return (2.0 * s) - a;
	}

	CONST_ANYTYPE_I farenheitToCelcius(T temperature) {
		return (temperature - 32.0) * (5.0/9.0);
	}

	CONST_ANYTYPE_I celciusToFarenheit(T temperature) {
		return (temperature / (5.0/9.0)) + 32.0;
	}

	template<Operatable T>
	constexpr inline std::function<T(T)> getInterpolationFunction(T from, T to) {
		return [=](T by) -> T {return (T)lerp(from, to, by);};
	}

	inline float dbToVolume(float db) {
		return powf(10.0f, 0.5f * db);
	}

	constexpr inline float volumeToDb(float volume) {
		return 20.0f * log10f(volume);
	}

	template<Type::Integer T = int>
	constexpr inline size_t uipow2(T val, size_t power) {
		return val << power;
	}

	template<Type::Integer T = int>
	constexpr inline bool bit(T val, size_t bit) {
		return (val & (0x1 << bit));
	}

	template <Operatable T>
	constexpr std::pair<int, T> frexp(T val) requires Type::Number<T> {
		int pow2 = 0;
		T rem = std::frexp((float)val, &pow2);
		return std::pair<int, T>{pow2, rem};
	}

	CONST_ANYTYPE_I ldexp(T val, int exp) requires Type::Number<T> {
		return (T)::ldexp((double)val, exp);
	}

	namespace Random {
		namespace {
			typedef std::uniform_real_distribution<double> RandReal;
			typedef std::uniform_int_distribution<size_t> RandLong;
			// The random number generator engine used
			std::default_random_engine	engine{Time::sinceEpoch<Time::Millis>()};
			// Default distributions
			RandReal	longDist(0, maxSizeT);
			RandLong	realDist(0.0,1.0);
		}

		/// Returns a random double between 0 and 1.
		double real() {
			return realDist(engine);
		}

		/// Returns a random double between the given values.
		template<Type::Real T = float>
		T real(T min, T max) {
			RandReal dist(min, max);
			return (T)dist(engine);
		}

		/// Returns a random integer between 0 and maximum size_t.
		size_t integer() {
			return longDist(engine);
		}

		/// Returns a random integer between the given values.
		template<Type::Integer T = int>
		T integer(T min, T max) {
			RandLong dist(min, max);
			return (T)dist(engine);
		}

		/// Sets the random number generator's seed.
		inline void setSeed(size_t seed) {
			engine = std::default_random_engine{seed};
		}

		/// Gets a seed based on the current clock's time, and a random number.
		inline size_t getNewSeed() {
			return Time::sinceEpoch<Time::Millis>() ^ !(integer() << 0x2F);
		}
	}
}

#ifndef nrtn
#define nrtn(val) Math::nrtn(val);
#endif // nrtn

#ifndef nroot
#define nroot(val, root) Math::nroot(val, root)
#endif // nroot

#define $mth Math::
#define $rng Math::Random::

#define RNG	Math::Random

#undef CONST_ANYTYPE_I
#undef CONST_ANYTYPE
#undef ANYTYPE_I
#undef ANYTYPE

#endif // ALGEBRA_MATH_MATHEMATICS_H

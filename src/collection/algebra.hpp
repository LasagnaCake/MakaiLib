#ifndef ALGEBRA_MATH_MATHEMATICS_H
#define ALGEBRA_MATH_MATHEMATICS_H

#include <string>
#include <cmath>
#include <random>
#include <limits>
#include <chrono>
#include "conceptual.hpp"

#define ANYTYPE			template<Operatable T> T
#define ANYTYPE_I		template<Operatable T> inline T
#define CONST_ANYTYPE	template<Operatable T> constexpr T
#define CONST_ANYTYPE_I	template<Operatable T> constexpr inline T

#define $maxof(TYPE) (std::numeric_limits<TYPE>::max())

#ifndef nroot
#define nroot(val, root) powf(val, 1.0/root)
#endif // nroot

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
	constexpr const double	hsqrt2		= sqrt2 / 2;
	constexpr const double	ln2			= 0.6931471805599453094172321215;
	constexpr const double	pi			= 3.1415926535897932384626433833;
	constexpr const double	hpi			= pi / 2;
	constexpr const double	tau			= 6.2831853071795864769252867666;
	constexpr const double	euler		= 2.7182818284590452353602874714;
	constexpr const double	degrad		= 180.0 / pi;
	constexpr const double	maribel		= euler - (sqrt2 - 1.0) * 1.2;
	constexpr const size_t	maxSizeT	= $maxof(size_t);
	constexpr const double	infinity	= std::numeric_limits<double>::infinity();
	constexpr const float	infinityF	= std::numeric_limits<float>::infinity();
	constexpr const double	epsilon		= 1.0 / infinity;
	constexpr const float	epsilonF	= 1.0 / infinityF;

	#ifndef SQRT2
	#define SQRT2 $mth sqrt2
	#endif // SQRT2

	#ifndef HSQRT2
	#define HSQRT2 $mth hsqrt2
	#endif // HSQRT2

	#ifndef LN2
	#define LN2 $mth ln2
	#endif // LN2

	#ifndef PI
	#define PI $mth pi
	#endif // PI

	#ifndef HPI
	#define HPI $mth hpi
	#endif // HPI

	#ifndef TAU
	#define TAU $mth tau
	#endif // TAU

	#ifndef EULER
	#define EULER $mth euler
	#endif // EULER

	#ifndef DEGRAD
	#define DEGRAD $mth degrad
	#endif // DEGRAD

	#ifndef MARIBEL
	#define MARIBEL $mth maribel
	#endif // DEGRAD

	CONST_ANYTYPE_I min(T a, T b) {
		return ((a < b) ? a : b);
	}

	CONST_ANYTYPE_I max(T a, T b) {
		return ((a > b) ? a : b);
	}

	CONST_ANYTYPE_I clamp(T a, T min, T max) {
		return ((a < min) ? min : ((a > max) ? max : a));
	}

	CONST_ANYTYPE_I radians(T deg) {
		return (T)(deg / degrad);
	}

	CONST_ANYTYPE_I degrees(T rad) {
		return (T)(rad * degrad);
	}

	CONST_ANYTYPE_I linearToRadian(T val) {
		return (T)(val * tau);
	}

	CONST_ANYTYPE_I radianToLinear(T val) {
		return (T)(val / tau);
	}

	CONST_ANYTYPE_I linearToDegree(T val) {
		return (T)(val * 360.0);
	}

	CONST_ANYTYPE_I degreeToLinear(T val) {
		return (T)(val / 360.0);
	}

	CONST_ANYTYPE_I lerp(T from, T to, T by) {
		return (T)(from + by * (to - from));
	}

	CONST_ANYTYPE angleLerp(T from, T to, T by) {
		T dist	= fmod(to - from, tau);
		dist	= fmod(2.0 * dist, tau) - dist;
		return (T)(from + dist * by);
	}

	CONST_ANYTYPE_I step(T val, T step) {
		if (step != 0)
			return (T)(floor(val / step + 0.5) * step);
		return val;
	}

	CONST_ANYTYPE floor(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Floor it
		return (T)(::floor(val * zeros) / zeros);
	}

	CONST_ANYTYPE ceil(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Ceil it
		return (T)(::ceil(val * zeros) / zeros);
	}

	CONST_ANYTYPE_I round(T val) {
		// Add 1/2 & floor it
		return (T)(::floor(val + 0.5));
	}

	CONST_ANYTYPE round(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Add 1/2 & floor it
		return (T)(::floor((val + 0.5) * zeros) / zeros);
	}

	CONST_ANYTYPE_I fmult(T val, T mult) {
		return ::floor(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I cmult(T val, T mult) {
		return ::ceil(val / (double)mult) * mult;
	}

	CONST_ANYTYPE_I wmax(T val, T max) {
		return val - fmult(val, max);
	}

	CONST_ANYTYPE_I wmin(T val, T min) {
		return val + cmult(val, min);
	}

	CONST_ANYTYPE_I wrap(T val, T min, T max) {
		return wmax(val + min, max) - min;
	}

	CONST_ANYTYPE_I nrtn(T val) {
		return (T)nroot(val, fabs(val));
	}

	CONST_ANYTYPE_I sign(T val) {
		return (val < 0 ? -1 : (val > 0 ? +1 : 0));
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
	* polygon of "radius" R (the shape's circumradius) at a given angle.
	*/
	constexpr float polarPolyPoint(
		float rotation,
		float angle,
		float sides,
		float radius
		) {
		sides /= 2;
		float
			sa2r = (angle * sides) / 2 + rotation,
			s2rs = (radius * sqrt2) / sides;
		float
			aCos = -abs(cos(sa2r)),
			aSin = -abs(sin(sa2r));
		return radius + s2rs * (aSin + aCos) / 2;
	}

	/// Reflects a given angle in relation to a surface.
	constexpr float reflection(float pointAngle, float surfaceAngle) {
		return pi + (2.0 * pointAngle) - surfaceAngle;
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
	constexpr std::pair<int, T> frexp(T val) {
		int pow2 = 0;
		T rem = std::frexp((float)val, &pow2);
		return std::pair<int, T>{pow2, rem};
	}

	CONST_ANYTYPE_I ldexp(T val, int exp) {
		return (T)::ldexp((double)val, exp);
	}

	namespace Random {
		namespace {
			typedef std::uniform_real_distribution<double> RandReal;
			typedef std::uniform_int_distribution<size_t> RandLong;
			// I think I just saw hell.
			size_t _getSeed() {
				auto time =
					std::chrono::duration_cast<std::chrono::microseconds>(
						std::chrono::system_clock::now()
						.time_since_epoch()
					);
				return time.count();
			}
			// The random number generator engine used
			std::default_random_engine	engine{_getSeed()};
			// Default distributions
			RandReal	longDist(0, maxSizeT);
			RandLong	realDist(0.0,1.0);
		}

		/// Returns a random double between 0 and 1.
		double real() {
			return realDist(engine);
		}

		/// Returns a random double between the given values.
		template<Type::Real T>
		T real(T min, T max) {
			RandReal dist(min, max);
			return (T)dist(engine);
		}

		/// Returns a random integer between 0 and maximum size_t.
		size_t integer() {
			return longDist(engine);
		}

		/// Returns a random integer between the given values.
		template<Type::Integer T>
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
			return _getSeed() ^ !(integer() << 0x2F);
		}
	}
}

#ifndef nrtn
#define nrtn(val) $mth nrtn(val);
#endif // nrtn

#define $mth Math::
#define $rng Math::Random::

#undef CONST_ANYTYPE_I
#undef CONST_ANYTYPE
#undef ANYTYPE_I
#undef ANYTYPE

#endif // ALGEBRA_MATH_MATHEMATICS_H

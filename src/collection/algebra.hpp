#ifndef ALGEBRA_MATH_MATHEMATICS_H
#define ALGEBRA_MATH_MATHEMATICS_H

#include <string>
#include <cmath>
#include <random>
#include <limits>
#include <chrono>

#define ANYTYPE		template<typename T> T
#define ANYTYPE_I	template<typename T> inline T

#define $maxof(TYPE) (std::numeric_limits<TYPE>::max())

#ifndef nroot
#define nroot(val, root) pow(val, 1.0/root)
#endif // nroot

namespace Math {
	/// Math Constants.
	const auto sqrt2		= 1.4142135623730950488016887242;
	const auto hsqrt2		= sqrt2 / 2;
	const auto ln2			= 0.6931471805599453094172321215;
	const auto pi			= 3.1415926535897932384626433833;
	const auto hpi			= pi / 2;
	const auto tau			= 6.2831853071795864769252867666;
	const auto euler		= 2.7182818284590452353602874714;
	const auto degrad		= 180.0 / pi;
	const auto maribel		= euler - (sqrt2 - 1.0) * 1.2;
	const size_t maxSizeT	= $maxof(size_t);
	const double infinity	= maxSizeT;
	const double epsilon	= 1.0 / infinity;

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

	ANYTYPE_I min(T a, T b) {
		return ((a < b) ? a : b);
	}

	ANYTYPE_I max(T a, T b) {
		return ((a > b) ? a : b);
	}

	ANYTYPE_I clamp(T a, T min, T max) {
		return ((a < min) ? min : ((a > max) ? max : a));
	}

	ANYTYPE_I radians(T deg) {
		return (T)(deg / degrad);
	}

	ANYTYPE_I degrees(T rad) {
		return (T)(rad * degrad);
	}

	ANYTYPE_I lerp(T from, T to, T by) {
		return (T)(from + by * (to - from));
	}

	ANYTYPE angleLerp(T from, T to, T by) {
		T dist	= fmod(to - from, tau);
		dist	= fmod(2.0 * dist, tau) - dist;
		return (T)(from + dist * by);
	}

	ANYTYPE_I step(T val, T step) {
		if (step != 0)
			return (T)(floor(val / step + 0.5) * step);
		return val;
	}

	ANYTYPE floor(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Floor it
		return (T)(::floor(val * zeros) / zeros);
	}

	ANYTYPE ceil(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Ceil it
		return (T)(::ceil(val * zeros) / zeros);
	}

	ANYTYPE_I round(T val) {
		// Add 1/2 & floor it
		return (T)(::floor(val + 0.5));
	}

	ANYTYPE round(T val, int decimals = 0) {
		// Get rounding factor
		T zeros = pow(10, decimals);
		// Add 1/2 & floor it
		return (T)(::floor((val + 0.5) * zeros) / zeros);
	}

	ANYTYPE_I fmult(T val, T mult) {
		return ::floor(val / (double)mult) * mult;
	}

	ANYTYPE_I cmult(T val, T mult) {
		return ::ceil(val / (double)mult) * mult;
	}

	ANYTYPE_I wmax(T val, T max) {
		return val - fmult(val, max);
	}

	ANYTYPE_I wmin(T val, T min) {
		return val + cmult(val, min);
	}

	ANYTYPE_I nrtn(T val) {
		return (T)nroot(val, fabs(val));
	}

	inline size_t digitCount(size_t number) {
		return std::to_string(number).size();
	}

	inline size_t digit(size_t val, unsigned char digit) {
		std::string vstr = std::to_string(val);
		return vstr[(vstr.size()-1) - digit] - '0';
	}

	/**
	* Returns the polar radius of a point along the edges of a N-sided
	* polygon of "radius" R (the shape's circumradius) at a given angle.
	*/
	float polarPolyPoint(
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
	float reflection(float pointAngle, float surfaceAngle) {
		return pi + (2.0 * pointAngle) - surfaceAngle;
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
			RandReal	longDist(0, $maxof(size_t));
			RandLong	realDist(0.0,1.0);
		}

		/// Returns a random double between 0 and 1.
		double real() {
			return realDist(engine);
		}

		/// Returns a random double between the given values.
		double real(double min, double max) {
			RandReal dist(min, max);
			return dist(engine);
		}

		/// Returns a random integer between 0 and maximum size_t.
		size_t integer() {
			return longDist(engine);
		}

		/// Returns a random integer between the given values.
		ANYTYPE integer(T min, T max) {
			RandLong dist(min, max);
			(T)dist(engine);
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

#undef ANYTYPE_I
#undef ANYTYPE

#endif // ALGEBRA_MATH_MATHEMATICS_H

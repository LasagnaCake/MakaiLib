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

namespace Math {
	/// Math Constants.
	const auto sqrt2		= 1.4142135623730950488016887242;
	const auto hsqrt2		= sqrt2 / 2;
	const auto ln2			= 0.6931471805599453094172321215;
	const auto pi			= 3.1415926535897932384626433833;
	const auto hpi			= pi / 2;
	const auto tau			= 6.2831853071795864769252867666;
	const auto euler		= 2.7182818284590452353602874714;
	const auto maribel		= euler - (sqrt2 - 1.0) * 1.2;
	const size_t maxSizeT	= 0xffffffffffffffff;
	const double infinity	= maxSizeT;
	const double epsilon	= 1 / infinity;

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
		return (T)(deg * pi / 180.0);
	}

	ANYTYPE_I degrees(T rad) {
		return (T)(rad * 180.0 / pi);
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

	namespace Random {
		namespace {
			typedef std::uniform_real_distribution<double> RandReal;
			typedef std::uniform_int_distribution<size_t> RandLong;
			// I think I just saw hell.
			size_t getSeed() {
				auto time =
					std::chrono::duration_cast<std::chrono::microseconds>(
						std::chrono::system_clock::now()
						.time_since_epoch()
					);
				return time.count();
			}
			// If device does not support true randomness, use fallback
			std::random_device			active;
			std::default_random_engine	fallback{getSeed()};
			// Default distributions
			RandReal	longDist(0, $maxof(size_t));
			RandLong	realDist(0.0,1.0);
		}

		/// Returns whether true randomness is available.
		inline bool trueRandom() {
			return active.entropy();
		}

		/// Returns a random double between 0 and 1.
		double real() {
			if (trueRandom())
				return realDist(active);
			else
				return realDist(fallback);
		}

		/// Returns a random double between the given values.
		double real(double min, double max) {
			RandReal dist(min, max);
			if (trueRandom())
				return dist(active);
			else
				return dist(fallback);
		}

		/// Returns a random integer between 0 and maximum size_t.
		size_t integer() {
			if (trueRandom())
				return longDist(active);
			else
				return longDist(fallback);
		}

		/// Returns a random integer between the given values.
		ANYTYPE integer(T min, T max) {
			RandLong dist(min, max);
			if (trueRandom())
				return (T)dist(active);
			else
				return (T)dist(fallback);
		}
	}
}

#define $mth Math::

#undef ANYTYPE_I
#undef ANYTYPE

#endif // ALGEBRA_MATH_MATHEMATICS_H

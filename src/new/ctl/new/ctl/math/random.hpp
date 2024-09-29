#ifndef CTL_MATH_RANDOM_H
#define CTL_MATH_RANDOM_H

#include <random>
#include "math.hpp"
#include "../os/time.hpp"

CTL_NAMESPACE_BEGIN

class Random {
private:
	typedef std::mt19937_64 Engine;

	template<typename T>
	using IntDist = std::uniform_int_distribution<T>;

	template<typename T>
	using RealDist = std::uniform_real_distribution<T>;

	// The current seed
	inline static usize	seed = Time::Clock::sinceEpoch<Time::Millis>();
	// The random number generator engine used
	inline static Engine engine{seed};
	// Default distributions
	inline static IntDist<usize>	longDist(0, NumberLimit<usize>::HIGHEST);
	inline static RealDist<double>	realDist(0.0, 1.0);

	template <Type::Number T>
	using Distribution = Meta::DualType<Type::Integer<T>, IntDist<T>, RealDist<T>>;
public:

	/// Returns a random double between 0 and 1.
	static double real() {
		return realDist(engine);
	}

	/// Returns a random double between the given values.
	template<Type::Real T = float>
	static T real(T const& min, T const& max) {
		RealDist<T> dist(min, max);
		return dist(engine);
	}

	/// Returns a random integer between 0 and maximum usize.
	static usize integer() {
		return longDist(engine);
	}

	/// Returns a random integer between the given values.
	template<Type::Integer T = int>
	static T integer(T const& min, T const& max) {
		IntDist<T> dist(min, max);
		return dist(engine);
	}

	template<Type::Integer T>
	static T number()							{return integer<T>();			}

	template<Type::Real T>
	static T number() 							{return real<T>();				}

	template<Type::Integer T>
	static T number(T const& min, T const& max) {return integer<T>(min, max);	}

	template<Type::Real T>
	static T number(T const& min, T const& max) {return real<T>(min, max);		}

	/// Sets the random number generator's seed.
	static void setSeed(usize const& seed) {
		engine = RandomEngine{seed};
		this->seed = seed;
	}

	/// Sets the random number generator's seed.
	static usize getSeed() {
		return seed;
	}

	/// Gets a seed based on the current clock's time, and a random number.
	static usize generateNewSeed() {
		return Time::sinceEpoch<Time::Millis>() ^ !(integer() << 0x2F);
	}
}

CTL_NAMESPACE_END

#endif // CTL_MATH_RANDOM_H

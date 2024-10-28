#ifndef CTL_RANDOM_GENERATOR_H
#define CTL_RANDOM_GENERATOR_H

#include "../typetraits/traits.hpp"
#include "engine.hpp"

CTL_NAMESPACE_BEGIN

namespace Random {

namespace Base {
	template<class T>
	struct Generator;

	template <Type::Subclass<SimpleEngine> TEngine>
	struct Generator<TEngine> {
		using EngineType = TEngine;

		constexpr static bool SECURE = false;

		Generator(): engine()				{}
		Generator(usize const& seed)		{engine.setSeed(seed);						}
		Generator(Generator const& other) 	{engine.setSeed(other.engine.getSeed());	}
		Generator(Generator&& other)		= default;

		/// Returns a random double between 0 and 1.
		template<Type::Real T = float>
		T real() {
			return static_cast<T>(engine.next()) / static_cast<T>(NumberLimit<usize>::HIGHEST);
		}

		/// Returns a random integer between 0 and highest usize.
		template<Type::Integer T = ssize>
		T integer() {
			return engine.next();
		}

		/// Sets the random number generator's seed.
		void setSeed(usize const& seed) {
			engine.setSeed(seed);
		}

		/// Gets the random number generator's current seed.
		usize getSeed() {
			return engine.getSeed();
		}

		/// Gets a seed based on the current clock's time, and a random number.
		usize generateNewSeed() {
			return OS::Time::time() ^ !(integer() << 0x2F);
		}
		
	protected:
		EngineType engine;
	};

	template <Type::Subclass<SecureEngine> TEngine>
	struct Generator<TEngine> {
		using EngineType = TEngine;

		constexpr static bool SECURE = true;

		Generator(): engine()							{}

		Generator(Generator const& other)				= delete;
		Generator(Generator&& other)					= default;

		Generator& operator=(Generator const& other)	= delete;
		Generator& operator=(Generator&& other)			= default;

		/// Returns a random double between 0 and 1.
		template<Type::Real T = float>
		T real() {
			return static_cast<T>(engine.next()) / static_cast<T>(NumberLimit<usize>::HIGHEST);
		}

		/// Returns a random integer between 0 and highest usize.
		template<Type::Integer T = ssize>
		T integer() {
			return engine.next();
		}
		
	protected:
		EngineType engine;
	};
}

template <class TEngine>
struct BaseGenerator: Base::Generator<TEngine> {
	using
		Base::Generator<TEngine>::Generator,
		Base::Generator<TEngine>::engine,
		Base::Generator<TEngine>::real,
		Base::Generator<TEngine>::integer
	;

	/// Returns a random double between the given values.
	template<Type::Real T = float>
	T real(T const& min, T const& max) {
		if (min > max) return real<T>(max, min);
		return real<T>() * (max - min) + min;
	}

	/// Returns a random integer between the given values.
	template<Type::Integer T = ssize>
	T integer(T const& min, T const& max) {
		if (min > max) return integer<T>(max, min);
		return (integer<T>() % (max - min)) + min;
	}

	template<Type::Integer T = ssize>
	T number()								{return integer<T>();			}

	template<Type::Real T = float>
	T number() 								{return real<T>();				}

	template<Type::Integer T = ssize>
	T number(T const& min, T const& max)	{return integer<T>(min, max);	}

	template<Type::Real T = float>
	T number(T const& min, T const& max)	{return real<T>(min, max);		}
};

using SimpleGenerator = BaseGenerator<Engine::Mersenne>;
using SecureGenerator = BaseGenerator<Engine::Secure>;

using Generator = SimpleGenerator;

}

CTL_NAMESPACE_END

#endif
#ifndef CTL_MATH_RANDOM_H
#define CTL_MATH_RANDOM_H

#include <random>
#include "core.hpp"
#include "../os/time.hpp"
#include "../container/string.hpp"

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(CTL_NO_WINDOWS_PLEASE)
#include <windows.h>
//#include <bcrypt.h>
#else
#endif

CTL_NAMESPACE_BEGIN

namespace Type::Random {
	template<class T>
	concept PseudoEngine = requires (T t) {
		{t.setSeed(0)}	-> Type::Void;
		{t.getSeed()}	-> Type::Convertible<usize>;
	};
}

namespace Random {

template<bool S>
struct Engine;

template<>
struct Engine<false> {
	constexpr static bool SECURE = false;
	virtual usize next() = 0;
	virtual usize getSeed() = 0;
	virtual void setSeed(usize const& seed) = 0;
	virtual ~Engine() {}
};

template<>
struct Engine<true> {
	constexpr static bool SECURE = true;
	virtual usize next() = 0;
	virtual ~Engine() {}
};

struct PseudoEngine: Engine<false> {
private:
	using DistributionType = std::uniform_int_distribution<usize>;

	using InternalEngine = Meta::DualType<sizeof(usize) == sizeof(uint64), std::mt19937_64, std::mt19937>;

	inline static DistributionType dist = DistributionType(0, NumberLimit<usize>::HIGHEST);
	
	usize seed;
	InternalEngine engine;
public:
	PseudoEngine(usize const& seed):	seed(seed), engine(seed)		{}
	PseudoEngine():						PseudoEngine(OS::Time::time())	{}

	PseudoEngine(PseudoEngine const& other)				= default;
	PseudoEngine(PseudoEngine&& other)					= default;

	PseudoEngine& operator=(PseudoEngine const& other)	= default;
	PseudoEngine& operator=(PseudoEngine&& other)		= default;

	virtual ~PseudoEngine() {}

	virtual usize getSeed() final {
		return seed;
	}

	virtual void setSeed(usize const& newSeed) final {
		seed = newSeed;
		engine = InternalEngine{seed};
	}

	virtual usize next() final {
		return dist(engine);
	}
};

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(CTL_NO_WINDOWS_PLEASE)
struct SecureAlgorithm {
private:
	BCRYPT_ALG_HANDLE algHandle;
public:
	SecureAlgorithm(): SecureAlgorithm(L"RNG")		{}

	SecureAlgorithm(SecureAlgorithm&& other)					= default;
	SecureAlgorithm(SecureAlgorithm const& other)				= delete;

	SecureAlgorithm& operator=(SecureAlgorithm&& other)			= default;
	SecureAlgorithm& operator=(SecureAlgorithm const& other)	= delete;

	SecureAlgorithm(WideString const& name) {
		BCryptOpenAlgorithmProvider(&algHandle, name.cstr(), NULL, 0);
	}

	operator BCRYPT_ALG_HANDLE() const {
		return algHandle;
	}

	~SecureAlgorithm() {
		BCryptCloseAlgorithmProvider(&algHandle, 0);
	}
};

struct SecureEngine: Engine<true> {
private:
	SecureAlgorithm algorithm;

public:
	SecureEngine()	{algorithm = SecureAlgorithm();}

	SecureEngine(SecureEngine&& other)					= default;
	SecureEngine(SecureEngine const& other)				= delete;

	SecureEngine& operator=(SecureEngine&& other)		= default;
	SecureEngine& operator=(SecureEngine const& other)	= delete;

	virtual ~SecureEngine() {}

	virtual usize next() final {
		usize num = 0;
		BCryptGenRandom(
			algorithm,
			(byte*)&num,
			sizeof(usize),
			0
		);
		return num;
	}
};
#else
struct SecureEngine: Engine<true> {
	virtual usize next() final {return 0;}
};
#endif

namespace Base {
	template<class T>
	struct RandomGenerator;

	template<>
	struct RandomGenerator<void> {};

	template <Type::Subclass<Engine<false>> TEngine>
	struct RandomGenerator<TEngine> {
		using EngineType = TEngine;

		constexpr static bool SECURE = false;

		RandomGenerator(): engine()						{}
		RandomGenerator(usize const& seed)				{engine.setSeed(seed);						}
		RandomGenerator(RandomGenerator const& other) 	{engine.setSeed(other.engine.getSeed());	}
		RandomGenerator(RandomGenerator&& other)		= default;

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

	template <Type::Derived<Engine<true>> TEngine>
	struct RandomGenerator<TEngine> {
		using EngineType = TEngine;

		constexpr static bool SECURE = true;

		RandomGenerator(): engine()									{}

		RandomGenerator(RandomGenerator const& other)				= delete;
		RandomGenerator(RandomGenerator&& other)					= default;

		RandomGenerator& operator=(RandomGenerator const& other)	= delete;
		RandomGenerator& operator=(RandomGenerator&& other)			= default;

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
struct BaseGenerator: Base::RandomGenerator<TEngine> {
	using
		Base::RandomGenerator<TEngine>::RandomGenerator,
		Base::RandomGenerator<TEngine>::engine,
		Base::RandomGenerator<TEngine>::real,
		Base::RandomGenerator<TEngine>::integer
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

using PseudoGenerator = BaseGenerator<PseudoEngine>;
using SecureGenerator = BaseGenerator<SecureEngine>;

using Generator = PseudoGenerator;

}

CTL_NAMESPACE_END

#endif // CTL_MATH_RANDOM_H

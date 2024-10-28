#ifndef CTL_RANDOM_ENGINE_H
#define CTL_RANDOM_ENGINE_H

#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(CTL_NO_WINDOWS_PLEASE)
#include <windows.h>
//#include <bcrypt.h>
#else
#endif

#include <random>

#include "../math/math.hpp"
#include "../os/time.hpp"

CTL_NAMESPACE_BEGIN

namespace Random {

namespace Base {
	struct Engine {
		virtual usize next() = 0;
	};

	struct SimpleEngine: Engine {
		constexpr static bool SECURE = false;
		virtual usize next() = 0;
		virtual usize getSeed() = 0;
		virtual void setSeed(usize const& seed) = 0;
		virtual ~SimpleEngine() {}
	};

	struct SecureEngine: Engine {
		constexpr static bool SECURE = true;
		virtual usize next() = 0;
		virtual ~SecureEngine() {}
	};
}

namespace Engine {
	struct Mersenne: Base::SimpleEngine {
	private:
		using DistributionType = std::uniform_int_distribution<usize>;

		using InternalEngine = Meta::DualType<sizeof(usize) == sizeof(uint64), std::mt19937_64, std::mt19937>;

		inline static DistributionType dist = DistributionType(0, NumberLimit<usize>::HIGHEST);
		
		usize seed;
		InternalEngine engine;
	public:
		Mersenne(usize const& seed):	seed(seed), engine(seed)	{}
		Mersenne():						Mersenne(OS::Time::time())	{}

		Mersenne(Mersenne const& other)				= default;
		Mersenne(Mersenne&& other)					= default;

		Mersenne& operator=(Mersenne const& other)	= default;
		Mersenne& operator=(Mersenne&& other)		= default;

		virtual ~Mersenne() {}

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
	struct Secure: Base::SecureEngine {
	private:
		struct Algorithm {
		private:
			BCRYPT_ALG_HANDLE algHandle;
		public:
			Algorithm() {
				BCryptOpenAlgorithmProvider(&algHandle, L"RNG", NULL, 0);
			}

			Algorithm(Algorithm&& other)					= default;
			Algorithm(Algorithm const& other)				= delete;

			Algorithm& operator=(Algorithm&& other)			= default;
			Algorithm& operator=(Algorithm const& other)	= delete;

			operator BCRYPT_ALG_HANDLE() const {
				return algHandle;
			}

			~Algorithm() {
				BCryptCloseAlgorithmProvider(&algHandle, 0);
			}
		};

		Algorithm algo;

	public:
		Secure(): algo()						{}

		Secure(Secure&& other)					= default;
		Secure(Secure const& other)				= delete;

		Secure& operator=(Secure&& other)		= default;
		Secure& operator=(Secure const& other)	= delete;

		virtual ~Secure() {}

		virtual usize next() final {
			usize num = 0;
			BCryptGenRandom(
				algo,
				(byte*)&num,
				sizeof(usize),
				0
			);
			return num;
		}
	};
	#else
	struct Secure: Base::SecureEngine {
		virtual usize next() final {return 0;}
	};
	#endif
}

}

CTL_NAMESPACE_END

#endif // CTL_RANDOM_ENGINE_H

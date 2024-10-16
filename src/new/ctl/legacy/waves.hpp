#ifndef WAVE_FUNCTIONS_H
#define WAVE_FUNCTIONS_H

#include "algebra.hpp"
#include "helper.hpp"

namespace Wave {
	template<typename T = float>
	using WaveFunction = Function<T(T const&, size_t const&)>;

	namespace {
		using
			Math::absin,
			Math::abcos,
			Math::ramp,
			Math::round,
			Math::Operatable,
			Math::fract
		;
	}

	#define WAVE_FUNCTION(NAME) template<Operatable T = float> constexpr T NAME(T const& t, size_t const& lod = 8)

	WAVE_FUNCTION(square)	{return sin(t) < 0 ? T(-1) : T(+1);	}

	WAVE_FUNCTION(sine)			{return sin(t);							}
	WAVE_FUNCTION(halfSine)		{return sin(ramp(t/PI  + 2.5) * PI);	}
	WAVE_FUNCTION(absSine)		{return absin(t);						}
	WAVE_FUNCTION(absHalfSine)	{return abs(halfSine(t));				}

	WAVE_FUNCTION(triangle)			{return (absin(t*0.5 + (HPI*0.5))-abcos(t*0.5 + (HPI*0.5)));	}
	WAVE_FUNCTION(halfTriangle)		{return ramp(t/PI  + 2.5) * 2.0;								}
	WAVE_FUNCTION(absTriangle)		{return abs(triangle(t));										}
	WAVE_FUNCTION(absHalfTriangle)	{return abs(halfTriangle(t));									}

	#undef WAVE_FUNCTION

	namespace Binary {
		#define BINARY_WAVE_FUNCTION(NAME)\
			template<Operatable T>\
			constexpr T NAME(T const& t, size_t const& lod) {\
				return (T)round<T>(::Wave::NAME(t) * lod) / lod;\
			}

		BINARY_WAVE_FUNCTION(sine			)
		BINARY_WAVE_FUNCTION(halfSine		)
		BINARY_WAVE_FUNCTION(absSine		)
		BINARY_WAVE_FUNCTION(absHalfSine	)

		BINARY_WAVE_FUNCTION(triangle			)
		BINARY_WAVE_FUNCTION(halfTriangle		)
		BINARY_WAVE_FUNCTION(absTriangle		)
		BINARY_WAVE_FUNCTION(absHalfTriangle	)

		#undef BINARY_WAVE_FUNCTION
	}
}

namespace Noise {
	template<typename T = float>
	using NoiseFunction = Function<T(T const&, size_t const&)>;

	namespace {
		using
			Math::fract,
			Math::Operatable
		;
	}

	#define NOISE_FUNCTION(NAME) template<Operatable T = float> constexpr T NAME(T const& t, size_t const& seed = 8)

	NOISE_FUNCTION(simple) {
		return fract<T>(sin(t * Math::mod((seed+sizeof(T)) / 12.9898, 78.233) * 43758.5453));
	}

	NOISE_FUNCTION(gold) {
		return fract<T>(tan((t*PHI - t) * seed));
	}

	NOISE_FUNCTION(super) {
		return gold<T>(simple<T>(t), seed);
	}

	#undef NOISE_FUNCTION
}

#endif // WAVE_FUNCTIONS_H

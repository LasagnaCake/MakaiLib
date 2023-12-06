#ifndef EASING_FUNCTIONS_H
#define EASING_FUNCTIONS_H

#include "algebra.hpp"

// Shut
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsequence-point"

// Taken from https://easings.net
namespace Ease {
	#define EASE_FUN(NAME) constexpr float NAME(float const& x)
	EASE_FUN(linear)	{return x;}
	/// In easing functions
	namespace In {
		EASE_FUN(linear)	{return x;}
		EASE_FUN(sine)		{return 1.0 - cos((x * PI) / 2.0);}
		EASE_FUN(quad)		{return x * x;}
		EASE_FUN(cubic)		{return x * x * x;}
		EASE_FUN(quart)		{return x * x * x * x;}
		EASE_FUN(quint)		{return x * x * x * x * x;}
		EASE_FUN(expo)		{return x == 0.0 ? 0.0 : pow(2.0, 10.0 * x - 10.0);}
		EASE_FUN(circ)		{return 1.0 - sqrt(1.0 - (x * x));}
		EASE_FUN(back)		{
			constexpr float c1 = 1.70158;
			constexpr float c3 = c1 + 1.0;
			return (c3 * x * x * x) - (c1 * x * x);
		}
		EASE_FUN(elastic)	{
			constexpr float c4 = (2.0 * PI) / 3.0;
			if (x == 0.0) return 0;
			if (x == 1.0) return 1;
			return -pow(2.0, 10.0 * x - 10.0) * sin((x * 10.0 - 10.75) * c4);
		}
		EASE_FUN(bounce)	{
			constexpr float n1 = 7.5625;
			constexpr float d1 = 2.75;
			float cx = 1.0 - x;
			if (x < 1.0 / d1)
				return 1.0 - (n1 * x * x);
			if (x < 2.0 / d1)
				return 1.0 - (n1 * (cx -= 1.5 / d1) * cx + 0.75);
			if (x < 2.5 / d1)
				return 1.0 - (n1 * (cx -= 2.25 / d1) * cx + 0.9375);
			return 1.0 - (n1 * (cx -= 2.625 / d1) * cx + 0.984375);
		}
	}
	/// Out easing functions
	namespace Out {
		EASE_FUN(linear)	{return x;}
		EASE_FUN(sine)		{return sin((x * PI) / 2.0);}
		EASE_FUN(quad)		{float const ix = 1.0 - x; return 1.0 - (ix * ix);}
		EASE_FUN(cubic)		{float const ix = 1.0 - x; return 1.0 - (ix * ix * ix);}
		EASE_FUN(quart)		{float const ix = 1.0 - x; return 1.0 - (ix * ix * ix * ix);}
		EASE_FUN(quint)		{float const ix = 1.0 - x; return 1.0 - (ix * ix * ix * ix * ix);}
		EASE_FUN(expo)		{return x == 1.0 ? 1.0 : 1.0 - pow(2.0, (-10.0) * x);}
		EASE_FUN(circ)		{return 1.0 - sqrt(1.0 - ((x-1.0) * (x-1.0)));}
		EASE_FUN(back)		{
			constexpr float c1 = 1.70158;
			constexpr float c3 = c1 + 1.0;
			float const cx = x - 1.0;
			return 1.0 + (c3 * cx * cx * cx) + (c1 * cx * cx);
		}
		EASE_FUN(elastic)	{
			constexpr float c4 = (2.0 * PI) / 3.0;
			if (x == 0.0) return 0;
			if (x == 1.0) return 1;
			return pow(2.0, (-10.0) * x) * sin((x * 10.0 - 0.75) * c4) + 1;
		}
		EASE_FUN(bounce)	{
			constexpr float n1 = 7.5625;
			constexpr float d1 = 2.75;
			float cx = x;
			if (x < 1.0 / d1)
				return n1 * x * x;
			if (x < 2.0 / d1)
				return n1 * (cx -= 1.5 / d1) * cx + 0.75;
			if (x < 2.5 / d1)
				return n1 * (cx -= 2.25 / d1) * cx + 0.9375;
			return n1 * (cx -= 2.625 / d1) * cx + 0.984375;
		}
	}
	/// In-Out easing functions
	namespace InOut {
		#define COMPOSITE_FUN(NAME) EASE_FUN(NAME) {return (x < 0.5) ? In::NAME(x*2.0)/2.0 : 0.5 + Out::NAME(x*2.0 -1)/2.0;}
		EASE_FUN(linear) {return x;}
		COMPOSITE_FUN(sine)
		COMPOSITE_FUN(quad)
		COMPOSITE_FUN(cubic)
		COMPOSITE_FUN(quart)
		COMPOSITE_FUN(quint)
		COMPOSITE_FUN(expo)
		COMPOSITE_FUN(circ)
		COMPOSITE_FUN(back)
		COMPOSITE_FUN(elastic)
		COMPOSITE_FUN(bounce)
		#undef COMPOSITE_FUN
	}
	/// Out-In easing functions
	namespace OutIn {
		#define COMPOSITE_FUN(NAME) EASE_FUN(NAME) {return (x < 0.5) ? Out::NAME(x*2.0)/2.0 : 0.5 + In::NAME(x*2.0 -1)/2.0;}
		EASE_FUN(linear) {return x;}
		COMPOSITE_FUN(sine)
		COMPOSITE_FUN(quad)
		COMPOSITE_FUN(cubic)
		COMPOSITE_FUN(quart)
		COMPOSITE_FUN(quint)
		COMPOSITE_FUN(expo)
		COMPOSITE_FUN(circ)
		COMPOSITE_FUN(back)
		COMPOSITE_FUN(elastic)
		COMPOSITE_FUN(bounce)
		#undef COMPOSITE_FUN
	}
	/// In-In easing functions
	namespace InIn {
		#define COMPOSITE_FUN(NAME) EASE_FUN(NAME) {return (x < 0.5) ? In::NAME(x*2.0)/2.0 : 0.5 + In::NAME(x*2.0 -1)/2.0;}
		EASE_FUN(linear) {return x;}
		COMPOSITE_FUN(sine)
		COMPOSITE_FUN(quad)
		COMPOSITE_FUN(cubic)
		COMPOSITE_FUN(quart)
		COMPOSITE_FUN(quint)
		COMPOSITE_FUN(expo)
		COMPOSITE_FUN(circ)
		COMPOSITE_FUN(back)
		COMPOSITE_FUN(elastic)
		COMPOSITE_FUN(bounce)
		#undef COMPOSITE_FUN
	}
	/// Out-Out easing functions
	namespace OutOut {
		#define COMPOSITE_FUN(NAME) EASE_FUN(NAME) {return (x < 0.5) ? Out::NAME(x*2.0)/2.0 : 0.5 + Out::NAME(x*2.0 -1)/2.0;}
		EASE_FUN(linear) {return x;}
		COMPOSITE_FUN(sine)
		COMPOSITE_FUN(quad)
		COMPOSITE_FUN(cubic)
		COMPOSITE_FUN(quart)
		COMPOSITE_FUN(quint)
		COMPOSITE_FUN(expo)
		COMPOSITE_FUN(circ)
		COMPOSITE_FUN(back)
		COMPOSITE_FUN(elastic)
		COMPOSITE_FUN(bounce)
		#undef COMPOSITE_FUN
	}
	#undef EASE_FUN
}

#pragma GCC diagnostic pop

#endif // EASING_FUNCTIONS_H

#ifndef VECTOR_COLOR_SPACE_H
#define VECTOR_COLOR_SPACE_H

#include "../anchors.hpp"

namespace Color{
	namespace {
		using namespace Vector;

		float _hueify(float t) {
			if(t < 0) t++;
            if(t > 1) t--;
            if(t < 1.0/6.0) return 6.0 * t;
            if(t < 1.0/2.0) return 1;
            if(t < 2.0/3.0) return (2.0/3.0 - t) * 6.0;
            return 0;
		}
	}
	// Custom color
	inline Vector4 Color(float r, float g, float b, float a = 1) {
		return Vector4(r, g, b, a);
	}
	inline Vector4 Color(float gray, float a = 1) {
		return Vector4(gray, gray, gray, a);
	}
	// Custom transparency level
	inline Vector4 ALPHA(float level) {
		return Vector4(1, 1, 1, Math::clamp(level, -0.1f, 1.0f));
	}

	// TODO: Optimize this
	Vector4 hueToPastel(float hue) {
		hue *= Math::tau;
		Vector3 res(
			cos(hue),
			cos(hue + Math::tau * (1.0/3.0)),
			cos(hue + Math::tau * (2.0/3.0))
		);
		res = (res^2.0).normalized() * Math::sqrt2;
		return Vector4(res.clamped(0, 1), 1);
	}

	Vector4 hueToRGB(float h) {
		h -= floor(h);
        return Vector4(
			_hueify(h + 1.0/3.0),
			_hueify(h),
			_hueify(h - 1.0/3.0),
			1
		);
	}

	Vector4 toRGB(float h, float s, float v, float a) {
		Vector4 res = hueToRGB(h);
		res *= (v * 2);
		Vector4 gray(Vector3((res.x + res.y + res.z) / 3), 1);
		res = Math::lerp(gray, res, Vector4(s));
		res.w = a;
		return res.clamped(0, 1);
	}

	// Transparency
	const Vector4 NONE			= Vector4(0);
	const Vector4 TRANSPARENT	= ALPHA(.0);
	const Vector4 MISTY			= ALPHA(.25);
	const Vector4 TRANSLUCENT	= ALPHA(.50);
	const Vector4 OPAQUE		= ALPHA(.75);
	const Vector4 SOLID			= ALPHA(1);
	// Luminance
	const Vector4 WHITE			= Color(1, 1);
	const Vector4 LIGHTGRAY		= Color(.75, 1);
	const Vector4 GRAY			= Color(.5, 1);
	const Vector4 DARKGRAY		= Color(.25, 1);
	const Vector4 BLACK			= Color(0, 1);
	// Primary Colors
	const Vector4 RED			= Color(1,	0,	0);
	const Vector4 GREEN			= Color(0,	1,	0);
	const Vector4 BLUE			= Color(0,	0,	1);
	// Secondary Colors
	const Vector4 YELLOW		= Color(1,	1,	0);
	const Vector4 MAGENTA		= Color(1,	0,	1);
	const Vector4 CYAN			= Color(0,	1,	1);
	// Tertiary Colors
	const Vector4 ORANGE		= Color(1,	.5,	0);
	const Vector4 AZURE			= Color(0,	.5,	1);
	const Vector4 TEAL			= Color(0,	1,	.5);
	const Vector4 LIME			= Color(.5,	1,	0);
	const Vector4 PURPLE		= Color(.5,	0,	1);
	const Vector4 PINK			= Color(1,	0,	.5);

	const Vector4 rainbow8[] = {
		PURPLE,
		MAGENTA,
		RED,
		ORANGE,
		YELLOW,
		LIME,
		GREEN,
		CYAN,
		BLUE
	};

	const Vector4 rainbow[] = {
		PURPLE,
		MAGENTA,
		PINK,
		RED,
		ORANGE,
		YELLOW,
		LIME,
		GREEN,
		TEAL,
		CYAN,
		AZURE,
		BLUE
	};
}

#define $col Color::

#endif

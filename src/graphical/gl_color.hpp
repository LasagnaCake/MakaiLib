#ifndef VECTOR_COLOR_SPACE_H
#define VECTOR_COLOR_SPACE_H

#include "../anchors.hpp"

namespace Color{
	namespace {
		using namespace Vector;
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
		return Vector4(1,1,1,Math::clamp(level, -0.1f, 1.0f));
	}
	// Transparency Levels
	const Vector4 NONE			= ALPHA(-.1);
	const Vector4 MISTY			= ALPHA(.25);
	const Vector4 TRANSLUCENT	= ALPHA(.50);
	const Vector4 OPAQUE		= ALPHA(.75);
	const Vector4 SOLID			= ALPHA(1);
	// Grays
	const Vector4 WHITE			= Color(1, 1);
	const Vector4 LIGHTGRAY		= Color(.75, 1);
	const Vector4 GRAY			= Color(.5, 1);
	const Vector4 DARKGRAY		= Color(.25, 1);
	const Vector4 BLACK			= Color(0, 1);
	// Primary Colors
	const Vector4 RED			= Color(1,	0,	0,	1);
	const Vector4 GREEN			= Color(0,	1,	0,	1);
	const Vector4 BLUE			= Color(0,	0,	1,	1);
	// Secondary Colors
	const Vector4 YELLOW		= Color(1,	1,	0,	1);
	const Vector4 MAGENTA		= Color(1,	0,	1,	1);
	const Vector4 CYAN			= Color(0,	1,	1,	1);
	// Tertiary Colors
	const Vector4 ORANGE		= Color(1,	.5,	0,	1);
	const Vector4 AZURE			= Color(0,	.5,	1,	1);
	const Vector4 TEAL			= Color(0,	1,	.5,	1);
	const Vector4 LIME			= Color(.5,	1,	0,	1);
	const Vector4 PURPLE		= Color(.5,	0,	1,	1);
	const Vector4 PINK			= Color(1,	0,	.5,	1);
}

#endif

#ifndef MAKAILIB_GRAPH_COLOR_H
#define MAKAILIB_GRAPH_COLOR_H

#include "../ctl/ctl.hpp"

namespace Makai::Graph::Color {

	// TODO: Optimize this
	constexpr Vector4 hueToPastel(float hue);

	constexpr Vector4 hueToRGB(float h);

	// Custom transparency level
	constexpr Vector4 alpha(float level);

	// Custom color
	constexpr Vector4 fromRGBA(float r, float g, float b, float a = 1);

	// Custom color
	constexpr Vector4 fromRGBA255(float r, float g, float b, float a = 255);

	constexpr Vector4 fromGrayscale(float gray, float a = 1);

	constexpr Vector4 fromGrayscale255(float gray, float a = 255);

	constexpr Vector4 fromHSL(float h, float s, float l, float a);

	constexpr Vector4 fromHexCodeRGBA(uint32 code);

	constexpr Vector4 fromHexCodeRGB(uint32 code);

	Vector4 fromHexCodeString(String code);

	constexpr uint32 toHexCodeRGB(uint32 const& color);

	constexpr uint32 toHexCodeRGBA(uint32 const& color);

	constexpr uint32 toHexCodeRGBA(Vector4 const& color);

	constexpr uint32 toHexCodeRGB(Vector4 const& color);

	constexpr String toHexCodeString(Vector4 const& color, bool const& toRGB = false, bool const& webColor = true);

	// Transparency
	constexpr Vector4 NONE			= Vector4(0);
	constexpr Vector4 CLEAR			= alpha(.0);
	constexpr Vector4 SEMIMISTY		= alpha(.125);
	constexpr Vector4 MISTY			= alpha(.25);
	constexpr Vector4 SEMILUCENT	= alpha(.375);
	constexpr Vector4 LUCENT		= alpha(.50);
	constexpr Vector4 SEMIMILKY		= alpha(.625);
	constexpr Vector4 MILKY			= alpha(.75);
	constexpr Vector4 SEMISOLID		= alpha(.875);
	constexpr Vector4 SOLID			= alpha(1);
	// Luminance
	constexpr Vector4 WHITE			= fromGrayscale(1, 1);
	constexpr Vector4 LIGHTGRAY		= fromGrayscale(.75, 1);
	constexpr Vector4 GRAY			= fromGrayscale(.5, 1);
	constexpr Vector4 DARKGRAY		= fromGrayscale(.25, 1);
	constexpr Vector4 BLACK			= fromGrayscale(0, 1);
	// Primary Colors
	constexpr Vector4 RED			= fromRGBA(1,	0,	0);
	constexpr Vector4 GREEN			= fromRGBA(0,	1,	0);
	constexpr Vector4 BLUE			= fromRGBA(0,	0,	1);
	// Secondary Colors
	constexpr Vector4 YELLOW		= fromRGBA(1,	1,	0);
	constexpr Vector4 MAGENTA		= fromRGBA(1,	0,	1);
	constexpr Vector4 CYAN			= fromRGBA(0,	1,	1);
	// Tertiary Colors
	constexpr Vector4 ORANGE		= fromRGBA(1,	.5,	0);
	constexpr Vector4 AZURE			= fromRGBA(0,	.5,	1);
	constexpr Vector4 TEAL			= fromRGBA(0,	1,	.5);
	constexpr Vector4 LIME			= fromRGBA(.5,	1,	0);
	constexpr Vector4 PURPLE		= fromRGBA(.5,	0,	1);
	constexpr Vector4 PINK			= fromRGBA(1,	0,	.5);

	constexpr Vector4 rainbow6[] = {
		MAGENTA,
		RED,
		YELLOW,
		GREEN,
		CYAN,
		BLUE
	};

	constexpr Vector4 rainbow8[] = {
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

	constexpr Vector4 rainbow[] = {
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

#endif // MAKAILIB_GRAPH_COLOR_H
